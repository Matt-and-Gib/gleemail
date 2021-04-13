#include "Arduino.h"
#include "src/include/global.h"

#include "src/include/display.h"
#include "src/include/morsecode.h"
#include "src/include/networking.h"


static Display& display = *new Display();
static Networking& network = *new Networking();
static InputMethod* input;
static unsigned short pinIndex = 0;
static char* userMessage = new char[MAX_MESSAGE_LENGTH + 1];
static char* peerMessage = new char[MAX_MESSAGE_LENGTH + 1];
bool pendingUserMessage = false;
bool pendingPeerMessage = false;


void getIncomingMessage() {
	if(network.messageAvailable()) {
		if(network.readMessage(peerMessage, MAX_MESSAGE_LENGTH)) {
			pendingPeerMessage = true;
		} else {

		}
	}
}


void processInputMethod() {
	pinIndex = 0;

	Pin **allPins = input->getPins();
	Pin *currentPin = allPins[pinIndex];
	while(*currentPin != NULL_PIN) {
		if(currentPin->mode == PIN_MODE::READ) {
			currentPin->value = digitalRead(currentPin->pinLocation);
		}

		currentPin = allPins[++pinIndex];
	}

	input->processInput(millis());

	pinIndex = 0;
	currentPin = allPins[pinIndex];
	while(*currentPin != NULL_PIN) {
		if(currentPin->mode == PIN_MODE::WRITE) {
			digitalWrite(currentPin->pinLocation, currentPin->value);
		}

		currentPin = allPins[++pinIndex];
	}
}


void updateUserMessage() {
	if(input->isMessageReady()) {
		input->getuserMessage(userMessage);
		pendingUserMessage = true;
	}
}


void updateDisplay() {
	if(pendingPeerMessage) {
		//Serial.print("Received: ");
		//Serial.println(peerMessage);
		display.updateReading(peerMessage);
	}

	if(pendingUserMessage) {
		//Serial.print("Your message: ");
		//Serial.println(userMessage);
		display.updateWriting(userMessage);
	}
}


void sendNetworkMessage() {
	if(pendingUserMessage) {
		if(!network.writeMessage(userMessage)) {
			
		}
	}
}


void printErrorCodes() {
	ERROR_CODE e = DebugLog::getLog().getError();
	while(e != ERROR_CODE::NONE) {
		if(!(OFFLINE_MODE && (e == NETWORK_INVALID_PEER_IP_ADDRESS))) {
			Serial.print("\nError Code: ");
			Serial.println(e);
		}

		e = DebugLog::getLog().getError();
	}
}


void loop() {
	getIncomingMessage();
	processInputMethod();
	updateUserMessage();
	updateDisplay();
	sendNetworkMessage();
	printErrorCodes();

	pendingUserMessage = false;
	pendingPeerMessage = false;
}


void setupPins() {
	Pin **pins = input->getPins();
	unsigned short i = 0;
	Pin *currentPin = pins[i];

	while (*currentPin != NULL_PIN) {
		/*Important debug messages. Check here first if something seems broken with hardware!
		Serial.print("Index: ");
		Serial.println(i);

		Serial.print(currentPin->pinLocation);
		Serial.print(" : ");
		Serial.println(currentPin->mode);*/

		pinMode(currentPin->pinLocation, currentPin->mode);
		currentPin = pins[++i];
	}
}


bool connectToWiFi() {
	unsigned short inputLength = 0;

	Serial.println("Enter WiFi SSID:");
	display.updateWriting("Enter SSID");
	char userSSID[network.getMaxSSIDLength() + 1];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userSSID, network.getMaxSSIDLength());
			break;
		}

		delay(250);
	}

	if(inputLength >= network.getMaxSSIDLength()) {
		DebugLog::getLog().logError(NETWORK_DATA_SSID_POSSIBLY_TRUNCATED);
	}
	userSSID[inputLength] = '\0';

	char userPassword[network.getMaxPasswordLength() + 1];
	Serial.print("Enter password for ");
	Serial.print(userSSID);
	Serial.println(":");
	display.updateWriting("Enter Password");
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userPassword, network.getMaxPasswordLength());
			break;
		}

		delay(250);
	}

	if(inputLength >= network.getMaxPasswordLength()) {
		DebugLog::getLog().logError(NETWORK_DATA_PASSWORD_POSSIBLY_TRUNCATED);
	}
	userPassword[inputLength] = '\0';

	Serial.println("Attempting connection...");
	display.updateWriting("Connecting...");

	if(!network.connectToNetwork(userSSID, userPassword)) {
		Serial.print("Unable to connect to ");
		display.updateWriting("Failed");
		Serial.println(userSSID);
		return false;
	}

	Serial.println("Connected!");
	display.updateWriting("Connected!");
	return true;
}


void connectToPeer() {
	char* ipAddressInputBuffer = new char[MAX_IP_ADDRESS_LENGTH + 1];
	char* ipAddressInputSubstringBuffer;
	uint8_t ipAddressParts[4];
	size_t ipAddressPartsIndex = 0;

	Serial.println("Enter your gleepal's IP address:");
	while(!(Serial.available() > 0)) {
		delay(250);
	}

	size_t readLength = Serial.readBytesUntil('\n', ipAddressInputBuffer, MAX_IP_ADDRESS_LENGTH);
	ipAddressInputBuffer[readLength] = '\0';
	while(Serial.available()) {
		Serial.read();
	}

	ipAddressInputSubstringBuffer = strtok(ipAddressInputBuffer, ".");
	while(ipAddressInputSubstringBuffer != nullptr) {
		ipAddressParts[ipAddressPartsIndex++] = atoi(ipAddressInputSubstringBuffer);
		ipAddressInputSubstringBuffer = strtok(NULL, ".");
	}

	IPAddress friendsIP(ipAddressParts[0], ipAddressParts[1], ipAddressParts[2], ipAddressParts[3]);

	Serial.print("Waiting for gleepal at ");
	Serial.print(friendsIP);
	Serial.println("...");

	if(!network.connectToPeer(friendsIP)) {
		Serial.println("Unable to connect to gleepal :(");
		return;
	}

	Serial.println("Connected to gleepal!");

	delete[] ipAddressInputSubstringBuffer;
	delete[] ipAddressInputBuffer;
}


bool setupInputMethod() {
	input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN);
	Serial.println("Downloading Input Method data...");

	char *data = network.downloadFromServer(input->getServerAddress(), input->getRequestHeaders());
	if(!data) {
		Serial.println("Unable to download data!");
		return false;
	}

	/*Serial.println("JSON Payload:\n");
	unsigned short i = 0;
	while(dat[i] != '\0') {
		Serial.print(dat[i++]);
	}
	Serial.println("\nDone");*/

	bool dataParsed = input->setNetworkData(data);
	delete[] data;
	return dataParsed;
}


void setup() {
	enum SETUP_LEVEL : short {WELCOME = 0, NETWORK = 1, INPUT_METHOD = 2, PINS = 3, PEER = 4, DONE = 5};
	SETUP_LEVEL setupState = WELCOME;
	bool setupComplete = false;

	const unsigned short SETUP_STEP_DELAY = 1500;

	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	do {
		switch(setupState) {
		case SETUP_LEVEL::WELCOME:
			Serial.println("Welcome to glEEmail!");
			Serial.print("Version ");
			Serial.println(GLEEMAIL_VERSION);
			Serial.println();

			if(OFFLINE_MODE) {
				Serial.println("Offline Mode Active\n");
			}

			display.updateReading("Hello, glEEmail!");
			setupState = SETUP_LEVEL::NETWORK;
		break;

		case SETUP_LEVEL::NETWORK:
			display.updateWriting("Joining WiFi");
			delay(SETUP_STEP_DELAY);

			display.clearWriting();
			display.updateReading("Joining WiFi");
			delay(SETUP_STEP_DELAY);

			if(connectToWiFi()) {
				setupState = SETUP_LEVEL::INPUT_METHOD;
			}
		break;

		case SETUP_LEVEL::INPUT_METHOD:
			display.updateReading("Setting up Input");
			display.updateWriting("Downloading Data");
			if(setupInputMethod()) {
				for(int i = 0; i < MAX_MESSAGE_LENGTH + 1; i += 1) {
					userMessage[i] = '\0';
					peerMessage[i] = '\0';
				}

				setupState = SETUP_LEVEL::PINS;
			}
		break;

		case SETUP_LEVEL::PINS:
			setupPins();
			setupState = SETUP_LEVEL::PEER;
		break;

		case SETUP_LEVEL::PEER:
			if(!OFFLINE_MODE) {
				display.updateWriting("Wait for glEEpal");
				connectToPeer();	
			}

			setupState = SETUP_LEVEL::DONE;
		break;

		case SETUP_LEVEL::DONE:
			Serial.println("Running");
			display.updateReading("Running");
			display.clearWriting();

			setupComplete = true;
		break;

		default:
			DebugLog::getLog().logError(ERROR_CODE::UNKNOWN_SETUP_STATE);
		break;
		}

		printErrorCodes();
		delay(SETUP_STEP_DELAY);
	} while (!setupComplete);

	display.clearAll();
}
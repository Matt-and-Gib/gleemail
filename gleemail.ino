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
bool pendingPeerMessage = false;


static long long frameStartTime = 0;
static long frameDuration = 0;
//static long greatestFrameDuration = 0;
static unsigned short frameLatencyCount = 0;


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

	input->processInput(frameStartTime);

	pinIndex = 0;
	currentPin = allPins[pinIndex];
	while(*currentPin != NULL_PIN) {
		if(currentPin->mode == PIN_MODE::WRITE) {
			digitalWrite(currentPin->pinLocation, currentPin->value);
		}

		currentPin = allPins[++pinIndex];
	}
}


void processNetwork() {
	network.processNetwork(frameStartTime);
}


/*void getIncomingMessage() {
	if(network.messageAvailable()) {
		if(network.readMessage(peerMessage, MAX_MESSAGE_LENGTH)) {
			pendingPeerMessage = true;
		} else {
			DebugLog::getLog().logError(NETWORK_PEER_MESSAGE_READ_FAILED);
		}
	}
}*/


void updateDisplay() {
	if(pendingPeerMessage) {
		//Serial.print("Received: ");
		//Serial.println(peerMessage);
		display.updateReading(peerMessage);
	}

	if(input->hasMessageChanged()) {
		input->peekUserMessage(userMessage); //Note: this will be wasted processing with different input method (i.e. TiltType)
		//Serial.print("Your message: ");
		//Serial.println(userMessage);
		display.updateWriting(userMessage);
	}
}


/*void sendNetworkMessage() {
	if(input->isMessageReady()) {
		input->getUserMessage(userMessage);
		//Serial.print("Your message: ");
		//Serial.println(userMessage);
		if(!network.writeMessage(userMessage)) {
			DebugLog::getLog().logError(NETWORK_WRITE_FAILED);
		}
	}
}*/


void printErrorCodes() {
	ERROR_CODE e = DebugLog::getLog().getError();
	while(e != ERROR_CODE::NONE) {
		if(!(OFFLINE_MODE && (e == NETWORK_INVALID_PEER_IP_ADDRESS))) {
			Serial.print(F("\nError Code: "));
			Serial.println(e);
		}

		e = DebugLog::getLog().getError();
	}
}

/*
	Estimated worst case frame: 180ms

	Process network max allocated time: 150ms

	Estimated max time for single message processing: 4ms

	Debounce time: 25ms
	Dot/Dash Threshold: 265ms


	180ms: totally unusable
	90ms: completely unusalbe
	45ms: unusable
	25ms: okay
	30ms: fine
	35ms: fine
	40ms: fine
	44ms: most unusable
	43ms: not really usable
	* 42ms: alright *
*/

void loop() {
	frameStartTime = millis();

	processInputMethod();
	processNetwork();
	updateDisplay();
	printErrorCodes();

	frameDuration = millis() - frameStartTime;
	if(frameDuration > MAX_FRAME_DURATION_MS) {
		//log error or something
		//but keep in mind that logging an error will make the next frame take longer
		frameLatencyCount += 1;
		if(frameLatencyCount > FRAME_LATENCY_COUNT_ERROR_THRESHOLD)
	} else {
		if(frameLatencyCount > 0) {
			frameLatencyCount = 0;
		}
	}

	pendingPeerMessage = false;
}


bool connectToWiFi() {
	unsigned short inputLength = 0;

	Serial.println(F("Enter WiFi SSID:"));
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
	Serial.print(F("Enter password for "));
	Serial.print(userSSID);
	Serial.println(F(":"));
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

	Serial.println(F("Attempting connection..."));
	display.updateWriting("Connecting...");

	if(!network.connectToNetwork(userSSID, userPassword)) {
		Serial.print(F("Unable to connect to "));
		display.updateWriting("Failed");
		Serial.println(userSSID);
		return false;
	}

	Serial.println(F("Connected!"));
	display.updateWriting("Connected!");
	return true;
}


bool setupInputMethod() {
	input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN);
	Serial.println(F("Downloading Input Method data..."));

	char *data = network.downloadFromServer(input->getServerAddress(), input->getRequestHeaders());
	if(!data) {
		Serial.println(F("Unable to download data!"));
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


void connectToPeer() {
	char* ipAddressInputBuffer = new char[MAX_IP_ADDRESS_LENGTH + 1];
	char* ipAddressInputSubstringBuffer;
	uint8_t ipAddressParts[4];
	size_t ipAddressPartsIndex = 0;

	Serial.println(F("Enter your gleepal's IP address:"));
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

	Serial.print(F("Waiting for gleepal at "));
	Serial.print(friendsIP);
	Serial.println(F("..."));

	if(!network.connectToPeer(friendsIP)) {
		Serial.println(F("Unable to connect to gleepal :("));
	} else {
		Serial.println(F("Connected to gleepal!"));
	}

	delete[] ipAddressInputSubstringBuffer;
	delete[] ipAddressInputBuffer;
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	while(Serial.available()) {
		Serial.read();
	}

	enum SETUP_LEVEL : short {WELCOME = 0, NETWORK = 1, INPUT_METHOD = 2, PINS = 3, PEER = 4, DONE = 5};
	SETUP_LEVEL setupState = WELCOME;
	bool setupComplete = false;

	const unsigned short SETUP_STEP_DELAY = 1500;

	do {
		switch(setupState) {
		case SETUP_LEVEL::WELCOME:
			Serial.println(F("Welcome to glEEmail!"));
			Serial.print(F("Version "));
			Serial.println(GLEEMAIL_VERSION);
			Serial.println();

			if(OFFLINE_MODE) {
				Serial.println(F("Offline Mode Active\n"));
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
			display.updateReading("Setting Up Input");
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
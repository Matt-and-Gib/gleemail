#include "Arduino.h"
#include "src/include/global.h"
#include "src/include/display.h"
#include "src/include/morsecode.h"
#include "src/include/networking.h"


static Display& display = *new Display();
static Networking& network = *new Networking();
static InputMethod* input;
static unsigned short pinIndex = 0;
static char* messageToSend = new char[MAX_MESSAGE_LENGTH + 1];
static char* messageReceived = new char[MAX_MESSAGE_LENGTH + 1];
bool pendingMessageToSend = false;
bool pendingMessageReceived = false;


void getIncomingMessage() {
	if(network.messageAvailable()) {
		if(network.readMessage(messageReceived, MAX_MESSAGE_LENGTH)) {
			pendingMessageReceived = true;
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


void updateMessageToSend() {
	if(input->isMessageReady()) {
		input->getMessageToSend(messageToSend);
		pendingMessageToSend = true;
	}
}


void updateDisplay() {
	//peek messageToSend
	//Push peek to inputMessage - write inputMessage to LCD
	if(pendingMessageReceived) {
		//Serial.print("Received: ");
		//Serial.println(messageReceived);
		display.updateReading(messageReceived);
	}

	if(pendingMessageToSend) {
		//Serial.print("message ready! : ");
		//input->getMessageToSend(messageToSend);
		//Serial.print("Your message: ");
		//Serial.println(messageToSend);
		display.updateWriting(messageToSend);
		//sendMessage(inputMethod->getMessageToSend());
	}
	//messageIn = receiveMessage();
	//printMessage(); //write to LCD buffer, I guess
}


void sendNetworkMessage() {
	if(pendingMessageToSend) {
		if(!network.writeMessage(messageToSend)) {
			
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
	updateMessageToSend();
	updateDisplay();
	sendNetworkMessage();
	printErrorCodes();

	pendingMessageToSend = false;
	pendingMessageReceived = false;
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

	if(!network.connectToNetwork(userSSID, userPassword)) {
		Serial.print("Unable to connect to ");
		Serial.println(userSSID);
		return false;
	}

	Serial.println("Connected!");
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
		printErrorCodes();
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
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	Serial.println("Welcome to glEEmail!");
	Serial.print("Version ");
	Serial.println(GLEEMAIL_VERSION);
	Serial.println();

	display.updateReading("Hello, glEEmail!");
	display.updateWriting("Joining WiFi");

	while(!connectToWiFi()) {
		delay(1000);
	}

	display.updateWriting("Downloading Data");
	if(!setupInputMethod()) {
		abort();
	}

	setupPins();

	for(int i = 0; i < MAX_MESSAGE_LENGTH + 1; i += 1) {
		messageToSend[i] = '\0';
		messageReceived[i] = '\0';
	}

	if(!OFFLINE_MODE) {
		display.updateWriting("Wait for glEEpal");
		connectToPeer();
	}

	display.clearAll();
	Serial.println("Running");
}
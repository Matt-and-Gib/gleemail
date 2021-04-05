#include "Arduino.h"
#include "src/include/global.h"
#include "src/include/morsecode.h"
#include "src/include/networking.h"


static Networking *network = new Networking();

static unsigned short pinIndex = 0;
static InputMethod *input;// = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN);
static char *messageOut;


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


void printErrorCodes() {
	ERROR_CODE e = DebugLog::getLog().getError();
	while(e != ERROR_CODE::NONE) {
		Serial.print("\nError Code: ");
		Serial.println(e);
		e = DebugLog::getLog().getError();
	}
}


void loop() {
	processInputMethod();
	//peek messageToSend
	//Push peek to inputMessage - write inputMessage to LCD
	if(input->isMessageReady()) {
		//Serial.print("message ready! : ");
		input->getMessageToSend(messageOut);
		Serial.println(messageOut);
		//sendMessage(inputMethod->getMessageToSend());
	}
	//messageIn = receiveMessage();
	//printMessage(); //write to LCD buffer, I guess

	printErrorCodes();
}


void setupPins() {
	Pin **pins = input->getPins();
	unsigned short i = 0;
	Pin *currentPin = pins[i];

	while (*currentPin != NULL_PIN) {
		/*Important debug messages. Check here first if something seems broken!
		Serial.print("Index: ");
		Serial.println(i);

		Serial.print(currentPin->pinLocation);
		Serial.print(" : ");
		Serial.println(currentPin->mode);*/

		pinMode(currentPin->pinLocation, currentPin->mode);
		currentPin = pins[++i];
	}
}


bool setupNetwork() {
	unsigned short inputLength = 0;

	Serial.println("Enter WiFi SSID:");
	char userSSID[network->getMaxSSIDLength() + 1];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userSSID, network->getMaxSSIDLength());
			break;
		}

		delay(250);
	}

	if(inputLength >= network->getMaxSSIDLength()) {
		DebugLog::getLog().logError(NETWORK_DATA_SSID_POSSIBLY_TRUNCATED);
	}

	userSSID[inputLength] = '\0';

	/*
		TODO: Check entered SSID against available SSIDs
	*/

	char userPassword[network->getMaxPasswordLength() + 1];
	Serial.print("Enter password for ");
	Serial.print(userSSID);
	Serial.println(":");
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userPassword, network->getMaxPasswordLength());
			break;
		}

		delay(250);
	}

	if(inputLength >= network->getMaxPasswordLength()) {
		DebugLog::getLog().logError(NETWORK_DATA_PASSWORD_POSSIBLY_TRUNCATED);
	}

	userPassword[inputLength] = '\0';

	Serial.println("Attempting connection...");

	if(!network->connectToNetwork(userSSID, userPassword)) {
		Serial.print("Unable to connect to ");
		Serial.println(userSSID);
		return false;
	}

	Serial.println("Connected!");
	return true;
}


bool setupInputMethod() {
	input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN);
	Serial.println("Downloading Input Method data...");
	//input->setNetworkData(network->downloadFromServer(input->getServerAddress(), input->getRequestHeaders()));
	char *data = network->downloadFromServer(input->getServerAddress(), input->getRequestHeaders());
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

	input->setNetworkData(data);
	delete[] data;

	return true;
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	while(!setupNetwork()) {
		delay(250);
	}

	if(!setupInputMethod()) {
		abort();
	}

	setupPins();

	messageOut = new char[MAX_MESSAGE_LENGTH];
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		messageOut[i] = '\0';
	}

	Serial.println("Running");
}
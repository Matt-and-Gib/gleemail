#include "Arduino.h"
#include "src/include/global.h"
#include "src/include/morsecode.h"
#include "src/include/networking.h"


static Networking* network = new Networking();
static InputMethod* input;// = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN);
static unsigned short pinIndex = 0;
static char* messageOut = new char[MAX_MESSAGE_LENGTH];


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


bool connectToWiFi() {
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

	if(!network->connectToPeer(friendsIP)) {
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

	while(!connectToWiFi()) {
		delay(1000);
	}

	if(!setupInputMethod()) {
		abort();
	}

	setupPins();

	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		messageOut[i] = '\0';
	}

	connectToPeer();

	Serial.println("Running");
}
#include "Arduino.h"
#include "src/include/global.h"
#include "src/include/preferences.h"

#include "src/include/storage.h"
#include "src/include/display.h"
#include "src/include/morsecode.h"

#include "src/include/internetaccess.h"
#include "src/include/networking.h"
#include "src/include/webaccess.h"


const unsigned long getCurrentTimeMS();
void updateDisplayWithPeerChat(const char*);
void updateDisplayWithUserChat(const char*);


static Storage storage;
static Display display;

static InternetAccess internet;
static Networking network(&getCurrentTimeMS, &updateDisplayWithPeerChat, 0);
static WebAccess webAccess;

static InputMethod* input;
static unsigned short pinIndex = 0;
static char* userMessage = new char[MAX_MESSAGE_LENGTH + 1];
static char* peerMessage = new char[MAX_MESSAGE_LENGTH + 1];
bool pendingPeerMessage = false;

void (*doAsynchronousProcess)();

static long long cycleStartTime = 0;
static long cycleDuration = 0;
//static long greatestCycleDuration = 0;
static unsigned short cycleLatencyCount = 0;


void verifyInputMethodData() {
	
}


void updateInputMethod() {
	pinIndex = 0;

	Pin **allPins = input->getPins();
	Pin *currentPin = allPins[pinIndex];
	while(*currentPin != NULL_PIN) {
		if(currentPin->mode == PIN_MODE::READ) {
			currentPin->value = digitalRead(currentPin->pinLocation);
		}

		currentPin = allPins[++pinIndex];
	}

	input->processInput(cycleStartTime);

	pinIndex = 0;
	currentPin = allPins[pinIndex];
	while(*currentPin != NULL_PIN) {
		if(currentPin->mode == PIN_MODE::WRITE) {
			digitalWrite(currentPin->pinLocation, currentPin->value);
		}

		currentPin = allPins[++pinIndex];
	}
}


const unsigned long getCurrentTimeMS() {
	return millis();
}


void updateNetwork() {
	network.processNetwork();
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


void userMessageChanged(char* chat) {
	display.updateWriting(chat);
}


void sendChatMessage(char* chat) {
	network.sendChatMessage(chat);
}


void updateDisplayWithPeerChat(const char* messageBody) {
	Serial.println(messageBody);
	display.updateReading(messageBody);
}


/*void updateDisplayWithUserChat(const char* messageBody) {
	//input->peekUserMessage(userMessage); //Note: this will be wasted processing with different input method (i.e. TiltType)
	//Serial.print("Your message: ");
	//Serial.println(userMessage);
	//display.updateWriting(userMessage);
}*/


/*void updateDisplay() {
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
}*/


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
	ERROR_CODE e = DebugLog::getLog().getNextError();
	while(e != ERROR_CODE::NONE) {
		//if(!(OFFLINE_MODE && (e == NETWORK_HEARTBEAT_FLATLINE))) {
			Serial.print(F("\nError Code: "));
			Serial.println(e);
		//}

		e = DebugLog::getLog().getNextError();
	}
}

/*
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
	44ms: mostly unusable
	43ms: not really usable
	* 42ms: alright *
*/

void loop() {
	cycleStartTime = millis();

	updateInputMethod();
	updateNetwork();
	//updateDisplay();
	doAsynchronousProcess();
	printErrorCodes();

	cycleDuration = millis() - cycleStartTime;
	if(cycleDuration > MAX_FRAME_DURATION_MS) {
		cycleLatencyCount += 1;
		if(cycleLatencyCount > FRAME_LATENCY_COUNT_ERROR_THRESHOLD) {
			DebugLog::getLog().logError(CONTINUOUS_FRAME_LATENCY);
			cycleLatencyCount = 0; //Reset to help latency by eliminating guaranteed error code print
		}
	} else {
		cycleLatencyCount = 0;
	}
}


bool prepareStorage() {
	if(!storage.begin()) {
		return false;
	}

	char* preferencesData = storage.readFile(prefsPath);
	if(!preferencesData) {
		DebugLog::getLog().logWarning(ERROR_CODE::STORAGE_COULDNT_LOAD_PREFS);
	} else {
		Preferences::getPrefs().loadSerializedPrefs(preferencesData, storage.lastReadFileLength());
	}
	delete[] preferencesData;

	return true;
}


bool promptForNewWiFiCredentials() {
	/*unsigned short USER_INPUT_TIMESPAN = 100; //Move this sad lonely variable to its home in the clouds
	unsigned long timespan = millis() + USER_INPUT_TIMESPAN;
	char userInput[1];

	Serial.println(F("Would you like to use a new WiFi SSID & Password? (Y, N)"));
	while(millis() < timespan) {
		if(Serial.available() > 0) {
			Serial.readBytesUntil('\n', userInput, 1);
			break;
		}
	}

	if(userInput[0] == 'Y' || userInput[0] == 'y') {
		return true;
	} else {
		return false;
	}*/

	return false;
}


bool connectToWiFi(bool forceManual = false) {
	bool changedLoginInfo = forceManual;

	if(forceManual || (!Preferences::getPrefs().getWiFiSSID() || !Preferences::getPrefs().getWiFiPassword())) {
		char desiredSSID[internet.getMaxSSIDLength()];
		char desiredPassword[internet.getMaxPasswordLength()];
		unsigned short inputLength = 0;

		Serial.println(F("Enter WiFi SSID:"));
		display.updateWriting("Enter SSID");
		while(true) {
			if(Serial.available() > 0) {
				inputLength = Serial.readBytesUntil('\n', desiredSSID, internet.getMaxSSIDLength());
				break;
			}

			delay(250);
		}

		if(inputLength >= internet.getMaxSSIDLength()) {
			DebugLog::getLog().logError(INTERNET_ACCESS_SSID_POSSIBLY_TRUNCATED);
		}

		Preferences::getPrefs().setWiFiSSID(copyAndTerminateString(desiredSSID, inputLength));

		Serial.print(F("Enter password for "));
		Serial.print(Preferences::getPrefs().getWiFiSSID());
		Serial.println(F(":"));
		display.updateWriting("Enter Password");
		while(true) {
			if(Serial.available() > 0) {
				inputLength = Serial.readBytesUntil('\n', desiredPassword, internet.getMaxPasswordLength());
				break;
			}

			delay(250);
		}

		if(inputLength >= internet.getMaxPasswordLength()) {
			DebugLog::getLog().logError(INTERNET_ACCESS_PASSWORD_POSSIBLY_TRUNCATED);
		}

		Preferences::getPrefs().setWiFiPassword(copyAndTerminateString(desiredPassword, inputLength));

		changedLoginInfo = true;
	} else {
		changedLoginInfo = false;
	}

	Serial.println(F("Attempting connection..."));
	display.updateWriting("Connecting...");

	if(!internet.connectToNetwork(Preferences::getPrefs().getWiFiSSID(), Preferences::getPrefs().getWiFiPassword())) {
		display.updateWriting("Failed");
		Serial.print(F("Unable to connect to "));
		Serial.println(Preferences::getPrefs().getWiFiSSID());

		return false;
	}

	if(changedLoginInfo) {
		//storage.savePrefs();
		storage.writeFile(Preferences::getPrefs().serializePrefs(), prefsPath, false/*CHANGE TO TRUE WHEN ENCRYPTION IS READY, YO*/);
	}

	Serial.println(F("Connected!"));
	display.updateWriting("Connected!");
	return true;
}


bool setupInputMethod() {
	input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN, &userMessageChanged, &sendChatMessage);

	char* data = storage.readFile(morseCodeCharPairsPath);
	if(!data) {
		Serial.println(F("Downloading Input Method data..."));
		data = webAccess.downloadFromServer(internet, input->getServerAddress(), input->getRequestHeaders());
		if(!data) {
			Serial.println(F("Unable to download data!"));
			return false;
		}

		storage.writeFile(data, morseCodeCharPairsPath);
	} else {
		Serial.println(F("Data exists on SD card!"));

		/*StaticJsonDocument<16> filter;
		filter["size"] = true;

		StaticJsonDocument<16> sizeDoc;
		deserializeJson(sizeDoc, data, DeserializationOption::Filter(filter));
		const unsigned short mccpSize = sizeDoc["size"];

		DynamicJsonDocument mccpDoc(mccpSize);
		DeserializationError error = deserializeJson(mccpDoc, data);

		if(error) {
			Serial.println(error.f_str());
		}

		const char* letter;
		const char* phrase;
		for (ArduinoJson::JsonObject elem : mccpDoc["morsecodetreedata"].as<ArduinoJson::JsonArray>()) {
			letter = elem["symbol"];
			phrase = elem["phrase"];
			Serial.print(F("Adding: "));
			Serial.print(phrase);
			Serial.print(F(" : "));
			Serial.println(letter);
		}*/

		//send request for version info
		//doAsynchronousProcess = &verifyInputMethodData;
	}

	//check if MCCP is stored
	//no:
		//download from GitHub
		//store on SD
	//yes:
		//send request for version info
		//doAsynchronousProcess = &verifyInputMethodData;

	//input->setNetworkData(storage.getMorseCodeCharPairs());

	

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


void connectToPeer() { //Use PEER wait time to do asynchronousProcess
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

	/*storage.begin();
	if(!storage.clearSavedPrefs(1337)) {
		Serial.println(F("Unable to delete!"));
	}

	Serial.println(F("Done"));
	abort();*/

	enum SETUP_LEVEL : short {WELCOME = 0, STORAGE = 1, NETWORK = 2, INPUT_METHOD = 3, PINS = 4, PEER = 5, DONE = 6};
	SETUP_LEVEL setupState = WELCOME;
	bool setupComplete = false;
	bool networkForceNewCredentials = false;

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
			setupState = SETUP_LEVEL::STORAGE;
		break;


		case SETUP_LEVEL::STORAGE:
			if(!prepareStorage()) {
				DebugLog::getLog().logWarning(ERROR_CODE::STORAGE_NOT_DETECTED);
			}

			setupState = SETUP_LEVEL::NETWORK;
		break;


		case SETUP_LEVEL::NETWORK:
			Serial.println(F("Joining WiFi"));
			display.updateWriting("Joining WiFi");
			delay(SETUP_STEP_DELAY);

			display.clearWriting();
			display.updateReading("Joining WiFi");
			delay(SETUP_STEP_DELAY);

			if(connectToWiFi(networkForceNewCredentials)) {
				setupState = SETUP_LEVEL::INPUT_METHOD;
			} else {
				networkForceNewCredentials = promptForNewWiFiCredentials();
			}
		break;

/*
~7.0 sec
~2.5 sec
~2.0 sec
~1.7 sec
~1.4 sec
*/

		case SETUP_LEVEL::INPUT_METHOD:
			display.updateReading("Setting Up Input");
			display.updateWriting("Downloading Data");
			if(setupInputMethod()) {
				for(unsigned short i = 0; i < MAX_MESSAGE_LENGTH + 1; i += 1) {
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


		case SETUP_LEVEL::PEER: //Use PEER wait time to do asynchronousProcess
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
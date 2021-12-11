#define ARDUINO_MAIN
#include "Arduino.h"
#include "src/include/global.h"
#include "src/include/preferences.h"

#include "src/include/storage.h"
#include "src/include/display.h"
#include "src/include/morsecode.h"

#include "src/include/internetaccess.h"
#include "src/include/networking.h"
#include "src/include/webaccess.h"

//Weak empty variant initialization function.
//May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() {}

extern USBDeviceClass USBDevice;

extern "C" void __libc_init_array(void);

bool quit = false;

static Storage storage;

static InputMethod* input = nullptr;
static unsigned short pinIndex = 0;
static char* userMessage = new char[MAX_MESSAGE_LENGTH + 1];
static char* peerMessage = new char[MAX_MESSAGE_LENGTH + 1];

static InternetAccess internet;
static WebAccess webAccess;

char* messageToPrint = nullptr;
void connectedToPeerClearDisplay();
void updateDisplayWithPeerChat(char*);
static Networking network(&millis, &updateDisplayWithPeerChat, &connectedToPeerClearDisplay, 0, quit);

static Display display;

static long long cycleStartTime = 0;
static long cycleDuration = 0;
static unsigned short cycleLatencyCount = 0;


void clearSerialInputBuffer() {
	while(Serial.available() > 0) {
		Serial.read();
	}
}


void connectedToPeerClearDisplay() {
	display.clearAll();
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


void updateNetwork() {
	network.processNetwork();
}


void userMessageChanged(char* chat) {
	display.updateWriting(chat);
}


void sendChatMessage(char* chat) {
	network.sendChatMessage(chat);
}


void updateDisplayWithPeerChat(char* messageBody) {
	//Serial.println(messageBody);
	messageToPrint = messageBody;
}


void updateDisplay() {
	if(messageToPrint != nullptr) {
		display.updateReading(messageToPrint);
		delete[] messageToPrint;
		messageToPrint = nullptr;
	}
}


void printErrorCodes() {
	ERROR_CODE e = DebugLog::getLog().getNextError();
	while(e != ERROR_CODE::NONE) {
		if(!(OFFLINE_MODE && (e == NETWORK_HEARTBEAT_FLATLINE))) {
			Serial.print(F("\nError Code: "));
			Serial.println(e);
		}

		e = DebugLog::getLog().getNextError();
	}
}


void initializeNetworkCredentialsFromPreferences(char** desiredWiFiSSID, char** desiredWiFiPassword) {
	const char* tempWiFiSSID = Preferences::getPrefs().getWiFiSSID();
	if(tempWiFiSSID != nullptr && tempWiFiSSID[0] != '\0') {
		*desiredWiFiSSID = copyString(tempWiFiSSID, strlen(tempWiFiSSID));
	}

	const char* tempWiFiPassword = Preferences::getPrefs().getWiFiPassword();
	if(tempWiFiPassword != nullptr && tempWiFiPassword[0] != '\0') {
		*desiredWiFiPassword = copyString(tempWiFiPassword, strlen(tempWiFiPassword));
	}
}


bool preparePreferences() {
	const char* preferencesData = storage.readFile(prefsPath);
	if(!preferencesData) {
		DebugLog::getLog().logWarning(ERROR_CODE::STORAGE_COULDNT_LOAD_PREFS);
		delete[] preferencesData;

		return false;
	} else {
		Preferences::getPrefs().deserializePrefs(preferencesData, storage.lastReadFileLength());
		delete[] preferencesData;

		return true;
	}
}


void enterNewWiFiCredentials(char** desiredWiFiSSID, char** desiredWiFiPassword) {
	delete[] *desiredWiFiSSID;
	delete[] *desiredWiFiPassword;

	*desiredWiFiSSID = new char[InternetAccess::getMaxSSIDLength() + 1]; //The + 1 is for the null terminator added in enterNewWiFiCredentials().
	*desiredWiFiPassword = new char[InternetAccess::getMaxPasswordLength() + 1]; //The + 1 is for the null terminator added in enterNewWiFiCredentials().

	Serial.println(F("Enter WiFi SSID:"));
	display.updateWriting("Enter SSID");

	unsigned short ssidInputLength = 0;
	while(true) {
		if(Serial.available() > 0) {
			ssidInputLength = Serial.readBytesUntil('\n', *desiredWiFiSSID, InternetAccess::getMaxSSIDLength());
			break;
		}

		delay(250);
	}

	(*desiredWiFiSSID)[ssidInputLength] = '\0';

	if(ssidInputLength >= InternetAccess::getMaxSSIDLength()) { //NOTE: if user enters a value equal to or in excess of max ssid length, we don't know if the ssid was truncated or just max-length
		DebugLog::getLog().logError(INTERNET_ACCESS_SSID_POSSIBLY_TRUNCATED);
	}

	clearSerialInputBuffer();

	Serial.print(F("Enter password for "));
	Serial.print(*desiredWiFiSSID);
	Serial.println(F(":"));
	display.updateWriting("Enter Password");

	unsigned short passwordInputLength = 0;
	while(true) {
		if(Serial.available() > 0) {
			passwordInputLength = Serial.readBytesUntil('\n', *desiredWiFiPassword, InternetAccess::getMaxPasswordLength());
			break;
		}

		delay(250);
	}
	(*desiredWiFiPassword)[passwordInputLength] = '\0';

	if(passwordInputLength >= InternetAccess::getMaxPasswordLength()) { //NOTE: if user enters a value equal to or in excess of max password length, we don't know if the password was truncated or just max-length
		DebugLog::getLog().logError(INTERNET_ACCESS_PASSWORD_POSSIBLY_TRUNCATED);
	}

	clearSerialInputBuffer();
}


bool promptForNewWiFiCredentials(char** desiredWiFiSSID, char** desiredWiFiPassword, bool forceNewCredentials) {
	if(!forceNewCredentials) {
		const unsigned short PROMPT_FOR_NEW_WIFI_CREDENTIALS_TIMEOUT = 5000; //This variable is loathsome. Drastically shorten if it becomes a problem.
		unsigned long promptForNewWiFiCredentialsUserInputTimeout = millis() + PROMPT_FOR_NEW_WIFI_CREDENTIALS_TIMEOUT;
		char promptForNewWiFiCredentialsUserInput[1] = {'\0'}; //This needs to be an array for readBytes().

		Serial.println(F("Would you like to use a new WiFi SSID & Password? (Y, N)"));
		while(millis() < promptForNewWiFiCredentialsUserInputTimeout) {
			if(Serial.available() > 0) {
				Serial.readBytes(promptForNewWiFiCredentialsUserInput, 1);
				break;
			}
		}

		clearSerialInputBuffer();

		if(!(promptForNewWiFiCredentialsUserInput[0] == 'y' || promptForNewWiFiCredentialsUserInput[0] == 'Y')) {
			return false;
		}
	}
	
	enterNewWiFiCredentials(desiredWiFiSSID, desiredWiFiPassword);
	return true;
}


bool connectToWiFi(char* desiredWiFiSSID, char* desiredWiFiPassword) {
	/*
		goal: successfully connect to wifi

		step 1: check if ssid & password exist in preferences
			no: ask for input

		step 2: attempt connection

		if(connection was successful && did not ask for new input) {
			good to go!
		}

		if(connection was successful && asked for new input) {
			set preferences to new input
		}

		if(connection failed && asked for new input) {
			ask for input again (go back to top)
			aka, return false
		}

		if(connection failed && did not ask for new input) {
			clear preferences and
			return false
		}
	
		step 3: return connection successful?
	*/

	Serial.println(F("Attempting connection..."));
	display.updateWriting("Connecting...");

	if(!internet.connectToNetwork(desiredWiFiSSID, desiredWiFiPassword)) {
		display.updateWriting("Failed");
		Serial.print(F("Unable to connect to "));
		Serial.println(desiredWiFiSSID);

		return false;
	} else {
		Serial.println(F("Connected!"));
		display.updateWriting("Connected!");

		return true;
	}
}


/*bool connectToWiFi(bool& changedLoginInfo, bool forceManual = false) {
	//bool changedLoginInfo = forceManual;

	unsigned short ssidInputLength = strlen(Preferences::getPrefs().getWiFiSSID());
	unsigned short passwordInputLength = strlen(Preferences::getPrefs().getWiFiPassword());
	char* desiredSSID = copyAndTerminateString(Preferences::getPrefs().getWiFiSSID(), ssidInputLength);
	char* desiredPassword = copyAndTerminateString(Preferences::getPrefs().getWiFiPassword(), passwordInputLength);

	if(forceManual || ssidInputLength == 0 || passwordInputLength == 0) {
		//desiredSSID = new char[InternetAccess::getMaxSSIDLength()];
		//desiredPassword = new char[InternetAccess::getMaxPasswordLength()];

		

		changedLoginInfo = true;
	} else {
		changedLoginInfo = false;
	}

	Serial.println(F("Attempting connection..."));
	display.updateWriting("Connecting...");

	if(!internet.connectToNetwork(desiredSSID, desiredPassword)) {
		display.updateWriting("Failed");
		Serial.print(F("Unable to connect to "));
		Serial.println(Preferences::getPrefs().getWiFiSSID());

		if(changedLoginInfo) {
			delete[] desiredSSID;
			delete[] desiredPassword;
		}

		return false;
	}

	if(changedLoginInfo) {
		Preferences::getPrefs().setWiFiSSID(copyAndTerminateString(desiredSSID, ssidInputLength));
		Preferences::getPrefs().setWiFiPassword(copyAndTerminateString(desiredPassword, passwordInputLength));

		delete[] desiredSSID;
		delete[] desiredPassword;

		const char* prefsData = Preferences::getPrefs().serializePrefs();
		storage.writeFile(prefsData, prefsPath);
		delete[] prefsData;
	}

	Serial.println(F("Connected!"));
	display.updateWriting("Connected!");
	return true;
}*/


unsigned short getMorseCodeCharPairsVersion() {
	const char SERVER_REQUEST[] = "GET /Matt-and-Gib/gleemail/main/data/MorseCodeCharPairsVersion HTTP/1.1";
	const char* REQUEST_HEADERS[REQUEST_HEADERS_LENGTH] = {
		SERVER_REQUEST,
		NETWORK_HEADER_USER_AGENT,
		HOST,
		NETWORK_HEADER_ACCEPTED_RETURN_TYPE,
		NETWORK_HEADER_CONNECTION_LIFETIME,
		HEADER_TERMINATION,
		nullptr
	};

	if(!webAccess.sendRequestToServer(internet, input->getServerAddress(), REQUEST_HEADERS)) {
		return 0;
	}

	const char* data = webAccess.downloadFromServer(internet);
	if(!data) {
		Serial.println(F("Unable to download version data!"));
		delete[] data;
		return 0;
	}

	const unsigned short downloadedMorseCodeCharPairsVersion = atoi(data);

	//Serial.print(F("v: "));
	//Serial.println(downloadedMorseCodeCharPairsVersion);

	delete[] data;

	return downloadedMorseCodeCharPairsVersion;
}


const char* getMorseCodeCharPairsData() {
	if(!webAccess.sendRequestToServer(internet, input->getServerAddress(), input->getRequestHeaders())) {
		return nullptr;
	}

	const char* data = webAccess.downloadFromServer(internet);
	if(!data) {
		Serial.println(F("Unable to download data!"));
		delete[] data;
		return nullptr;
	}

	return data;
}


bool setupMorseCodeInputMethod() {
	display.updateWriting("Downloading Data");

	const unsigned short mccpVersion = getMorseCodeCharPairsVersion();
	const char* data = storage.readFile(morseCodeCharPairsPath);
	if(!data) {
		Serial.println(F("Downloading Input Method data..."));

		data = getMorseCodeCharPairsData();
		storage.writeFile(data, morseCodeCharPairsPath);

		Preferences::getPrefs().setMorseCodeCharPairsVersion(mccpVersion);
		const char* prefsData = Preferences::getPrefs().serializePrefs();
		storage.writeFile(prefsData, prefsPath);
		delete[] prefsData;
	} else {
		//webAccess.sendRequestToServer(internet, input->getServerAddress(), input->getRequestHeaders()); //REQUEST_HEADERS);
		//doAsynchronousProcess = &verifyInputMethodData;

		if(Preferences::getPrefs().getMorseCodeCharPairsVersion() != mccpVersion) {
			Serial.println(F("Morse Code Char Pairs Version Mismatch"));

			delete[] data;

			data = getMorseCodeCharPairsData();
			storage.writeFile(data, morseCodeCharPairsPath);

			Preferences::getPrefs().setMorseCodeCharPairsVersion(mccpVersion);
			const char* prefsData = Preferences::getPrefs().serializePrefs();
			storage.writeFile(prefsData, prefsPath);
			delete[] prefsData;
		}
	}

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

	Serial.println(F("Initializing Authentication..."));

	network.connectToPeer(friendsIP);

	Serial.print(F("Waiting for gleepal at "));
	Serial.print(friendsIP);
	Serial.println(F("..."));

	/*if(!network.connectToPeer(friendsIP)) {
		Serial.println(F("Unable to connect to gleepal :("));
	} else {
		Serial.println(F("Connected to gleepal!"));
	}*/

	//delete[] ipAddressInputSubstringBuffer;
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

	if(RESET_STORAGE) {
		storage.begin();
		storage.eraseAll(133769);
		Serial.println(F("Files deleted successfully"));
	}

	enum SETUP_LEVEL : short {WELCOME = 0, STORAGE = 1, NETWORK = 2, INPUT_METHOD = 3, PINS = 4, PEER = 5, DONE = 6};
	SETUP_LEVEL setupState = WELCOME;
	bool setupComplete = false;

	char* desiredWiFiSSID = nullptr;
	char* desiredWiFiPassword = nullptr;
	bool networkPromptForNewCredentials = false;
	bool networkCredentialsChanged = false;
	bool networkCredentialsExist = false;

	const unsigned short SETUP_STEP_DELAY = 0;

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
			if(storage.begin()) {
				if(preparePreferences()) {
					initializeNetworkCredentialsFromPreferences(&desiredWiFiSSID, &desiredWiFiPassword);
				}
			} else {
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

			networkCredentialsExist = desiredWiFiSSID && desiredWiFiPassword;
			if(!networkCredentialsExist || networkPromptForNewCredentials) {
				networkCredentialsChanged = promptForNewWiFiCredentials(&desiredWiFiSSID, &desiredWiFiPassword, !networkCredentialsExist);
			}

			Serial.print(F("SSID Length: "));
			Serial.println(strlen(desiredWiFiSSID));
			Serial.print(F("Password Length: "));
			Serial.println(strlen(desiredWiFiPassword));

			if(connectToWiFi(desiredWiFiSSID, desiredWiFiPassword)) {
				Serial.print(F("LocalIP: "));
				Serial.println(internet.getLocalIP());

				if(networkCredentialsChanged) {
					Preferences::getPrefs().setWiFiSSID(desiredWiFiSSID);
					Preferences::getPrefs().setWiFiPassword(desiredWiFiPassword);

					const char* prefsData = Preferences::getPrefs().serializePrefs();
					storage.writeFile(prefsData, prefsPath);
					delete[] prefsData;
				}

				setupState = SETUP_LEVEL::INPUT_METHOD;
			} else {
				networkPromptForNewCredentials = true;
			}
		break;


		case SETUP_LEVEL::INPUT_METHOD:
			display.updateReading("Setting Up Input");
			if(input == nullptr) {
				input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN, &userMessageChanged, &sendChatMessage);
			}

			if(setupMorseCodeInputMethod()) {
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
			display.clearWriting();
			display.updateReading("Wait for glEEpal");

			setupComplete = true;
		break;


		default:
			DebugLog::getLog().logError(ERROR_CODE::UNKNOWN_SETUP_STATE);
		break;
		}

		printErrorCodes();
		delay(SETUP_STEP_DELAY);
	} while (!setupComplete);

	//display.clearAll();
}


int main(void) {
	init();
	__libc_init_array();
	initVariant();

	delay(1);
#if defined(USBCON)
	USBDevice.init();
	USBDevice.attach();
#endif

	setup();

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

	while(!quit) {
		cycleStartTime = millis();

		updateInputMethod();
		updateNetwork();
		updateDisplay();
		//doAsynchronousProcess();
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

		if(arduino::serialEventRun) {
			arduino::serialEventRun();
		}
	}

	display.updateReading("glEEbye!");
	display.clearWriting();

	Serial.println(F("Lost connection to glEEpal"));
	if(arduino::serialEventRun) {
		arduino::serialEventRun();
	}

	delay(5000);
	display.clearAll();
	return 0;
}
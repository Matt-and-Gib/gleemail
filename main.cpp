#define ARDUINO_MAIN
#include "Arduino.h"
#include "src/include/global.h"
#include "src/include/preferences.h"

#include "src/include/queue.h"
#include "src/include/kvpair.h"

#include "src/include/display.h"
#include "src/include/storage.h"

#include "src/include/internetaccess.h"
#include "src/include/websiteaccess.h"

#include "src/include/networking.h"

#include "src/include/morsecode.h" //Manually change this if using different input method


using namespace GLEEMAIL_DEBUG;


//Weak empty variant initialization function.
//May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() {}

extern USBDeviceClass USBDevice;

extern "C" void __libc_init_array(void);


bool quit = false;

Display display;

Storage storage;

InternetAccess internet;
WebsiteAccess websiteAccess;

char* messageToPrint = nullptr;
void connectedToPeerClearDisplay();
void updateDisplayWithPeerChat(char*);
Networking network(&millis, &updateDisplayWithPeerChat, &connectedToPeerClearDisplay, 0, quit);

InputMethod* input = nullptr;
unsigned short pinIndex = 0;
char* userMessage = new char[MAX_MESSAGE_LENGTH + 1];
char* peerMessage = new char[MAX_MESSAGE_LENGTH + 1];

long long cycleStartTime = 0;
long cycleDuration = 0;
unsigned short cycleLatencyCount = 0;


void clearSerialInputBuffer() {
	while(Serial.available()) {
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


bool resetCodeEntered() {
	return storage.eraseAll(133769);
}


void checkStartupCodes() {
	if(Serial.available()) {
		if(Serial.peek() == '-') {
			Serial.read();
			switch(Serial.read()) { //Assuming a second character is available; if not, this will block!
			case 'R':
				if(resetCodeEntered()) {
					Serial.println(F("Files deleted successfully"));
				} else {
					Serial.println(F("Unable to erase SD card!")); //Should probably be an error code
				}
			break;

			case 't':
				Serial.println(F("Startup Code Test"));
			break;

			default:
				DebugLog::getLog().logError(ERROR_CODE::UNKNOWN_STARTUP_CODE);
			break;
			}

			//Handle additional codes?
		} else {
			DebugLog::getLog().logWarning(ERROR_CODE::INVALID_STARTUP_CODE);
		}

		clearSerialInputBuffer();
	}
}


void initializeNetworkCredentialsFromPreferences(char** desiredWiFiSSID, char** desiredWiFiPassword) {
	delete[] *desiredWiFiSSID;
	delete[] *desiredWiFiPassword;

	const char* tempWiFiSSID = Preferences::getPrefs().getWiFiSSID();
	if(tempWiFiSSID != nullptr && tempWiFiSSID[0] != '\0') {
		*desiredWiFiSSID = copyAndTerminateString(tempWiFiSSID, strlen(tempWiFiSSID));
	}

	const char* tempWiFiPassword = Preferences::getPrefs().getWiFiPassword();
	if(tempWiFiPassword != nullptr && tempWiFiPassword[0] != '\0') {
		*desiredWiFiPassword = copyAndTerminateString(tempWiFiPassword, strlen(tempWiFiPassword));
	}
}


bool preparePreferences() {
	const char* preferencesData = storage.readFile(Preferences::getPrefs().getPrefsPath());

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

	if(ssidInputLength >= InternetAccess::getMaxSSIDLength()) { //NOTE: if user enters a value equal to or in excess of max ssid length, we don't know if the ssid was truncated or just max-length. We COULD know this by checking if there is anything remaining on the input buffer.
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

	if(passwordInputLength >= InternetAccess::getMaxPasswordLength()) { //NOTE: if user enters a value equal to or in excess of max password length, we don't know if the password was truncated or just max-length. We COULD know if there is more by checking the input buffer here for available characters.
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


const char* getDataFromInternet(const char* requestEndpoint) {
	const char* headers[7] = {
		requestEndpoint,
		NETWORK_HEADER_USER_AGENT,
		NETWORK_HEADER_HOST,
		NETWORK_HEADER_ACCEPTED_RETURN_TYPE,
		NETWORK_HEADER_CONNECTION_LIFETIME,
		NETWORK_HEADER_TERMINATION,
		nullptr
	};

	if(!websiteAccess.sendRequestToServer(internet, SERVER, headers)) {
		return nullptr;
	}

	return websiteAccess.downloadFromServer(internet);
}


bool setupInputMethod() {
	display.updateWriting("Downloading Data");

	const char* rawVersionData = getDataFromInternet(input->getDataVersionRequestEndpoint());
	if(!rawVersionData) {
		return false;
	}
	
	const unsigned short inputMethodDataVersion = atoi(rawVersionData);
	delete[] rawVersionData;

	bool downloadFullDataPackage = true;
	const char* data = storage.readFile(input->getCachedDataPath());
	if(data != nullptr && Preferences::getPrefs().getMorseCodeCharPairsVersion() == inputMethodDataVersion) { //Make this input-method agnostic
		downloadFullDataPackage = false;
	}

	if(downloadFullDataPackage) {
		Serial.println(F("Downloading Input Method data..."));

		delete[] data;
		data = getDataFromInternet(input->getDataRequestEndpoint());

		storage.writeFile(data, input->getCachedDataPath());

		Preferences::getPrefs().setMorseCodeCharPairsVersion(inputMethodDataVersion); //Make this input-method agnostic
		const char* prefsData = Preferences::getPrefs().serializePrefs();
		storage.writeFile(prefsData, Preferences::getPrefs().getPrefsPath());
		delete[] prefsData;
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

	Serial.println(F("Initializing Authentication..."));
	display.updateReading("Initializing");
	display.updateWriting("  Authentication"); //Right aligned

	network.connectToPeer(friendsIP);

	Serial.print(F("Waiting for gleepal at "));
	Serial.print(friendsIP);
	Serial.println(F("..."));

	//delete[] ipAddressInputSubstringBuffer; //Not necessary because strtok modifies the original string (doesn't allocate memory)
	delete[] ipAddressInputBuffer;
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	clearSerialInputBuffer(); //This is probably unnecessary.

	enum SETUP_LEVEL : short {WELCOME = 0, STORAGE = 1, PREFERENCES = 2, NETWORK = 3, INPUT_METHOD = 4, PINS = 5, PEER = 6, DONE = 7};
	SETUP_LEVEL setupState = WELCOME;
	bool setupComplete = false;


	{ //TODO: think of way to pass member function pointer instead of function pointer
		Queue<KVPair<char, bool (*)(void)>> startupCodes; //this will live right here in Setup
		const char testChar = 'R'; //this would be a private member of an object (in this case Storage)
		startupCodes.enqueue(new KVPair<char, bool (*)(void)>(testChar, &resetCodeEntered)); //the queue will probably be passed in to the object's begin function for the object to enqueue its KVPairs

		/*
			When checkStartupCodes() is called and a code is found, loop through the startupCodes queue to see if there are any matches, and if so: call the value function pointer.
		*/

		//at the end of setup()
		/*//This is the "safe" way
		QueueNode<KVPair<char, bool (*)(void)>>* pair;
		while(pair = startupCodes.dequeue()) {
			delete pair;
		}*/
		//but I'm pretty sure the destructor for Queue will cascade-delete all children
	}


	char* desiredWiFiSSID = nullptr;
	char* desiredWiFiPassword = nullptr;
	bool networkPromptForNewCredentials = false;
	bool networkCredentialsChanged = false;
	bool networkCredentialsExist = false;

	const unsigned short SETUP_STEP_DELAY_MS = 100;
	const unsigned short HELLO_GLEEMAIL_DELAY_MS = 1337;
	const unsigned short NETWORK_FAILED_DELAY_MS = 3600; //Smallest GitHub rate limit is 1000/hour, and there are 3600000ms in one hour, therefore sending one request per 3600ms will hopefully ensure we dont' exceed any limits

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
			delay(HELLO_GLEEMAIL_DELAY_MS);
			setupState = SETUP_LEVEL::STORAGE;
		break;


		case SETUP_LEVEL::STORAGE:
			if(!storage.begin()) {
				DebugLog::getLog().logWarning(ERROR_CODE::STORAGE_NOT_DETECTED);
				setupState = SETUP_LEVEL::NETWORK;
			} else {
				storage.printAll(); //DEBUG_ONLY_REMOVE_ME
				setupState = SETUP_LEVEL::PREFERENCES;
			}
		break;


		case SETUP_LEVEL::PREFERENCES:
			if(preparePreferences()) {
				Serial.println(F("Prepare preferences succeeded; init network creds")); //DEBUG_ONLY_REMOVE_ME
				initializeNetworkCredentialsFromPreferences(&desiredWiFiSSID, &desiredWiFiPassword);
			} else {
				Serial.println(F("Prepare prefs failed!")); //DEBUG_ONLY_REMOVE_ME
			}

			setupState = SETUP_LEVEL::NETWORK;
		break;


		case SETUP_LEVEL::NETWORK:
			Serial.println(F("Joining WiFi"));
			display.updateWriting("Joining WiFi");
			delay(SETUP_STEP_DELAY_MS);

			display.clearWriting();
			display.updateReading("Joining WiFi");
			delay(SETUP_STEP_DELAY_MS);

			networkCredentialsExist = desiredWiFiSSID && desiredWiFiPassword;
			if(!networkCredentialsExist || networkPromptForNewCredentials) {
				networkCredentialsChanged = promptForNewWiFiCredentials(&desiredWiFiSSID, &desiredWiFiPassword, !networkCredentialsExist);
			}

			if(connectToWiFi(desiredWiFiSSID, desiredWiFiPassword)) {
				if(networkCredentialsChanged) {
					Preferences::getPrefs().setWiFiSSID(desiredWiFiSSID);
					Preferences::getPrefs().setWiFiPassword(desiredWiFiPassword);

					const char* prefsData = Preferences::getPrefs().serializePrefs();
					storage.writeFile(prefsData, Preferences::getPrefs().getPrefsPath());
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
				input = new MorseCodeInput(LED_BUILTIN, &userMessageChanged, &sendChatMessage); //This assignment must be manually changed if a different input method is desired
			}

			if(setupInputMethod()) {
				for(unsigned short i = 0; i < MAX_MESSAGE_LENGTH + 1; i += 1) {
					userMessage[i] = '\0';
					peerMessage[i] = '\0';
				}

				setupState = SETUP_LEVEL::PINS;
			} else {
				delay(NETWORK_FAILED_DELAY_MS); //Delay so we don't get blocked by GitHub if download repeatedly fails.
			}
		break;


		case SETUP_LEVEL::PINS:
			setupPins();
			setupState = SETUP_LEVEL::PEER;
		break;


		case SETUP_LEVEL::PEER:
			if(!OFFLINE_MODE) {
				display.clearWriting();
				display.updateReading("Enter glEEpal IP");
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

		checkStartupCodes();
		printErrorCodes();
		delay(SETUP_STEP_DELAY_MS);
	} while (!setupComplete);
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

//NOTE: The below timing calculations are probably incorrect now that chat messages are encrypted/decrypted
/*
Estimated max time for single message processing: 4ms

	Debounce time: 25ms
	Dot/Dash Threshold: 265ms


	180ms: totally unusable
	90ms: completely unusable
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

		cycleDuration = millis() - cycleStartTime; //Remove to increase processing speed
		if(cycleDuration > MAX_FRAME_DURATION_MS) { //Remove to increase processing speed
			cycleLatencyCount += 1;
			if(cycleLatencyCount > FRAME_LATENCY_COUNT_ERROR_THRESHOLD) {
				DebugLog::getLog().logError(CONTINUOUS_FRAME_LATENCY);
				cycleLatencyCount = 0;
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
#define ARDUINO_MAIN
#include "Arduino.h"
#include "src/include/global.h"
#include "src/include/preferences.h"

#include "src/include/startupcodehandler.h"
#include "src/include/queue.h"
#include "src/include/keyvaluepair.h"

#include "src/include/corecomponent.h"

#include "src/include/display.h"
#include "src/include/storage.h"

#include "src/include/internetaccess.h"
#include "src/include/websiteaccess.h"

#include "src/include/networking.h"

#include "src/include/morsecode.h" //Manually change this if using different input method


#define GLEEMAIL_FRAME_TIMER


using namespace GLEEMAIL_DEBUG;


class VerboseModeEmissary final : public StartupCodeHandler {
private:
	const char VERBOSE_MODE_STARTUP_CODE = 'v';
	void verboseModeStartupCodeReceived() {DebugLog::getLog().enableVerboseMode();}
public:
	VerboseModeEmissary() = default;
	~VerboseModeEmissary() = default;

	void registerNewStartupCodes(Queue<KVPair<const char&, StartupCodeHandlerData* const>>& startupCodeHandlers) override {
		startupCodeHandlers.enqueue(new KVPair<const char&, StartupCodeHandlerData* const>(VERBOSE_MODE_STARTUP_CODE, new StartupCodeHandlerData(this, reinterpret_cast<void (StartupCodeHandler::*)(void)>(&VerboseModeEmissary::verboseModeStartupCodeReceived))));
	}


	void startupCodeReceived(void (StartupCodeHandler::*memberFunction)(void)) override {
		(this->*(reinterpret_cast<void (VerboseModeEmissary::*)(void)>(memberFunction)))();
	}
};


//Weak empty variant initialization function.
//May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() {}

extern USBDeviceClass USBDevice;

extern "C" void __libc_init_array(void);


#define COUNT_OF_CORE_COMPONENTS 3
#define CC_STORAGE_INDEX 0
#define CC_DISPLAY_INDEX 1
#define CC_NETWORKING_INDEX 2


const static constexpr unsigned short SERIAL_READ_LOOP_DELAY_MS = 250;
const unsigned short MAX_STARTUP_CODES = 7;
const unsigned short STARTUP_CODE_PROCESSED = 127;

char* messageToPrint = nullptr;

Display* display = nullptr;
Networking* network = nullptr;
InputMethod* input = nullptr;
unsigned short pinIndex = 0;
char* userMessage = new char[MAX_MESSAGE_LENGTH + TERMINATOR];
char* peerMessage = new char[MAX_MESSAGE_LENGTH + TERMINATOR];

long long cycleStartTime = 0;
long cycleDuration = 0;
unsigned short cycleLatencyCount = 0;


void clearSerialInputBuffer() {
	while(Serial.available()) {
		Serial.read();
	}
}


void connectedToPeerClearDisplay() {
	display->clearAll();
}


void userMessageChanged(char* chat) {
	display->updateWriting(chat);
}


void sendChatMessage(char* chat) { //remove
	network->sendChatMessage(chat);
}


void updateDisplayWithPeerChat(char* messageBody) {
	messageToPrint = messageBody;
}


void printErrorCodes() {
	if(DebugLog::getLog().getPendingErrors() > 0) {
		ERROR_CODE e = DebugLog::getLog().getNextError();
		do {
			Serial.print(F("\nError Code: "));
			Serial.println(e);

			e = DebugLog::getLog().getNextError();
		} while(e != ERROR_CODE::NONE);
	}
}


bool checkEnableStartupCodes() {
	if(Serial.available() > 1) {
		if(Serial.peek() == '-') {
			Serial.read();
			if(Serial.read() == 'A') { //Make 'A' a const variable to eliminate magic char
				if(Serial.peek() < ' ') {
					clearSerialInputBuffer();
				}

				return true;
			} else {
				DebugLog::getLog().logError(ERROR_CODE::STARTUP_CODE_MALFORMED_PROMPT_TRIGGER);
				clearSerialInputBuffer();
			}
		} else {
			DebugLog::getLog().logWarning(ERROR_CODE::STARTUP_CODE_UNEXPECTED_SERIAL_INPUT);
		}
	}	

	return false;
}


void setStartupCodes(char (& startupCodes)[MAX_STARTUP_CODES + TERMINATOR]) {
	while(!Serial.available()) {
		delay(SERIAL_READ_LOOP_DELAY_MS);
	}

	unsigned short codesCount = 0;
	char currentCode = 0;
	do {
		if(codesCount == MAX_STARTUP_CODES) {
			DebugLog::getLog().logError(ERROR_CODE::STARTUP_CODE_TOO_MANY_CODES_PROVIDED);
			break;
		}

		currentCode = Serial.read();
		if(currentCode < ' ') {
			break;
		}

		if(currentCode == ' ') {
			continue;
		} else {
			for(unsigned short index = 0; index < MAX_STARTUP_CODES; index += 1) {
				if(startupCodes[index] == 0) {
					startupCodes[index] = currentCode;
					codesCount += 1;
					break;
				} else {
					if(startupCodes[index] == currentCode) {
						DebugLog::getLog().logWarning(ERROR_CODE::STARTUP_CODE_DUPLICATE_CODES_NOT_ALLOWED);
						break;
					}
				}
			}
		}
	} while (Serial.available());

	clearSerialInputBuffer();
}


inline void checkStartupCodes(char (& codes)[MAX_STARTUP_CODES + TERMINATOR], Queue<KVPair<const char&, StartupCodeHandlerData* const>>& handlers) {
	for(unsigned short index = 0; index < MAX_STARTUP_CODES; index += 1) {
		if(codes[index] == STARTUP_CODE_PROCESSED) {
			continue;
		}

		QueueNode<KVPair<const char&, StartupCodeHandlerData* const>>* registeredHandler = handlers.peek();
		while(registeredHandler != nullptr) {
			if(*(registeredHandler->getData()) == codes[index]) {
				break;
			}

			registeredHandler = registeredHandler->getNode();
		}

		if(registeredHandler != nullptr) {
			registeredHandler->getData()->getValue()->instance->startupCodeReceived(registeredHandler->getData()->getValue()->callback);
			codes[index] = STARTUP_CODE_PROCESSED;
		}
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


bool preparePreferences(Storage* const storage) {
	if(!storage) {
		DebugLog::getLog().logError(ERROR_CODE::STORAGE_OBJECT_UNSUCCESSFULLY_ALLOCATED);
		return false;
	}

	const char* const preferencesData = storage->readFile(Preferences::getPrefs().getPrefsPath());

	if(!preferencesData) {
		DebugLog::getLog().logWarning(ERROR_CODE::PREFERENCES_LOAD_FAILED);
		delete[] preferencesData;

		return false;
	} else {
		const bool deserializeResult = Preferences::getPrefs().deserializePrefs(preferencesData, storage->lastReadFileLength());
		delete[] preferencesData;

		return deserializeResult;
	}
}


void enterNewWiFiCredentials(char** desiredWiFiSSID, char** desiredWiFiPassword) {
	delete[] *desiredWiFiSSID;
	delete[] *desiredWiFiPassword;

	*desiredWiFiSSID = new char[InternetAccess::getMaxSSIDLength() + TERMINATOR];
	*desiredWiFiPassword = new char[InternetAccess::getMaxPasswordLength() + TERMINATOR];

	Serial.println(F("Enter WiFi SSID:"));
	display->updateWriting("Enter SSID");

	unsigned short ssidInputLength = 0;
	while(true) {
		if(Serial.available() > 0) {
			ssidInputLength = Serial.readBytesUntil('\n', *desiredWiFiSSID, InternetAccess::getMaxSSIDLength());
			break;
		}

		delay(SERIAL_READ_LOOP_DELAY_MS);
	}

	(*desiredWiFiSSID)[ssidInputLength] = '\0';

	if(ssidInputLength >= InternetAccess::getMaxSSIDLength()) { //NOTE: if user enters a value equal to or in excess of max ssid length, we don't know if the ssid was truncated or just max-length. We COULD know this by checking if there is anything remaining on the input buffer.
		DebugLog::getLog().logError(INTERNET_ACCESS_SSID_POSSIBLY_TRUNCATED);
	}

	clearSerialInputBuffer();

	Serial.print(F("Enter password for "));
	Serial.print(*desiredWiFiSSID);
	Serial.println(F(":"));
	display->updateWriting("Enter Password");

	unsigned short passwordInputLength = 0;
	while(true) {
		if(Serial.available() > 0) {
			passwordInputLength = Serial.readBytesUntil('\n', *desiredWiFiPassword, InternetAccess::getMaxPasswordLength());
			break;
		}

		delay(SERIAL_READ_LOOP_DELAY_MS);
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


bool connectToWiFi(InternetAccess* const internet, const char* const desiredWiFiSSID, const char* const desiredWiFiPassword) {
	Serial.println(F("Attempting connection..."));
	display->updateWriting("Connecting...");

	if(!internet->connectToNetwork(desiredWiFiSSID, desiredWiFiPassword)) {
		display->updateWriting("Failed");
		Serial.print(F("Unable to connect to "));
		Serial.println(desiredWiFiSSID);

		return false;
	} else {
		Serial.println(F("Connected!"));
		display->updateWriting("Connected!");

		return true;
	}
}


const char* getDataFromInternet(const char* const requestEndpoint, InternetAccess* const internet) {
	const char* headers[7] = {
		requestEndpoint,
		NETWORK_HEADER_USER_AGENT,
		NETWORK_HEADER_HOST,
		NETWORK_HEADER_ACCEPTED_RETURN_TYPE,
		NETWORK_HEADER_CONNECTION_LIFETIME,
		NETWORK_HEADER_TERMINATION,
		nullptr
	};

	if(!WebsiteAccess::sendRequestToServer(*internet, SERVER, headers)) {
		return nullptr;
	}

	return WebsiteAccess::downloadFromServer(*internet);
}


bool setupInputMethod(InternetAccess* const internet, Storage* const storage) {
	display->updateWriting("Downloading Data");

	const char* rawVersionData = getDataFromInternet(input->getDataVersionRequestEndpoint(), internet);
	if(!rawVersionData) {
		DebugLog::getLog().logWarning(ERROR_CODE::INPUT_METHOD_DATA_VERSION_DOWNLOAD_FAILED);
		return false;
	}

	const unsigned short inputMethodDataVersion = atoi(rawVersionData);
	delete[] rawVersionData;
	rawVersionData = nullptr;

	bool downloadDataPackage = true;
	const char* data = nullptr;
	if(storage) {
		data = storage->readFile(input->getCachedDataPath());
	}

	if(data != nullptr && Preferences::getPrefs().getMorseCodeCharPairsVersion() == inputMethodDataVersion) { //Make this input-method agnostic
		downloadDataPackage = false;
	}

	if(downloadDataPackage) {
		Serial.println(F("Downloading Input Method data..."));

		delete[] data;
		data = getDataFromInternet(input->getDataRequestEndpoint(), internet);
		if(!data) {
			DebugLog::getLog().logWarning(ERROR_CODE::INPUT_METHOD_DATA_DOWNLOAD_FAILED);
			return false;
		}
	}

	bool dataParsed = input->setNetworkData(data);
	if(dataParsed && downloadDataPackage) {
		if(inputMethodDataVersion > Preferences::getPrefs().getMorseCodeCharPairsVersion()) {
			Preferences::getPrefs().setMorseCodeCharPairsVersion(inputMethodDataVersion); //Make this input-method agnostic
		}

		if(storage != nullptr) {
			if(!storage->writeFile(data, input->getCachedDataPath())) {
				DebugLog::getLog().logError(ERROR_CODE::STORAGE_WRITE_UNEXPECTEDLY_FAILED);
			}

			if(inputMethodDataVersion > Preferences::getPrefs().getMorseCodeCharPairsVersion()) {
				const char* prefsData = Preferences::getPrefs().serializePrefs();
				if(!storage->writeFile(prefsData, Preferences::getPrefs().getPrefsPath())) {
					DebugLog::getLog().logError(ERROR_CODE::STORAGE_WRITE_UNEXPECTEDLY_FAILED);
				}
				delete[] prefsData;
			}
		}
	}
	
	delete[] data;
	return dataParsed;
}


void setupPins() {
	Pin** pins = input->getPins();
	
	unsigned short i = 0;
	Pin* currentPin = pins[i];
	while (*currentPin != NULL_PIN) {
		pinMode(currentPin->pinLocation, currentPin->mode);
		currentPin = pins[++i];
	}
}


void connectToPeer(Networking& network) {
	char* ipAddressInputBuffer = new char[MAX_IP_ADDRESS_LENGTH + TERMINATOR];
	char* ipAddressInputSubstringBuffer;
	uint8_t ipAddressParts[4];
	size_t ipAddressPartsIndex = 0;

	Serial.println(F("Enter your gleepal's IP address:"));
	while(!(Serial.available() > 0)) {
		delay(SERIAL_READ_LOOP_DELAY_MS);
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
	display->updateReading("Initializing");
	display->updateWriting("  Authentication");

	network.initiateHandshake(friendsIP);

	Serial.print(F("Waiting for gleepal at "));
	Serial.println(friendsIP);

	delete[] ipAddressInputBuffer;
}


void setup(bool& quit, CoreComponent* coreComponents[COUNT_OF_CORE_COMPONENTS]) {
	enum class SETUP_LEVEL {BEGIN, SERIAL_COMM, DEBUG_LOG, STARTUP_CODES, LCD, WELCOME, STORAGE, PREFERENCES, NETWORKING, INPUT_METHOD, PINS, PEER, DONE};
	SETUP_LEVEL setupState = SETUP_LEVEL::BEGIN;

	const unsigned short BAUD_RATE = 9600;

	VerboseModeEmissary verboseModeEmissary;

	char startupCodes[MAX_STARTUP_CODES + TERMINATOR] {0};
	Queue<KVPair<const char&, StartupCodeHandlerData* const>> startupCodeHandlers;

	char* desiredWiFiSSID = nullptr;
	char* desiredWiFiPassword = nullptr;
	bool promptForNewNetworkCredentials = false;
	bool networkCredentialsChanged = false;
	bool networkCredentialsExist = false;

	const unsigned short SETUP_STEP_DELAY_MS = 100;
	const unsigned short STARTUP_CODE_INPUT_TIME_MS = 1500;
	const unsigned short HELLO_GLEEMAIL_DELAY_MS = 1000;
	const unsigned short NETWORK_FAILED_DELAY_MS = 3600; //Smallest GitHub rate limit is 1000/hour, and there are 3600000ms in one hour, therefore sending one request per 3600ms will hopefully ensure we dont' exceed any limits

	Storage* storage = nullptr;
	InternetAccess internet;

	do {
		switch(setupState) {
		case SETUP_LEVEL::BEGIN:
			setupState = SETUP_LEVEL::SERIAL_COMM;
		break;


		case SETUP_LEVEL::SERIAL_COMM:
			Serial.begin(BAUD_RATE);
			while(!Serial) {
				delay(SERIAL_READ_LOOP_DELAY_MS);
			}

			clearSerialInputBuffer();

			setupState = SETUP_LEVEL::DEBUG_LOG;
		break;


		case SETUP_LEVEL::DEBUG_LOG:
			verboseModeEmissary.registerNewStartupCodes(startupCodeHandlers);

			setupState = SETUP_LEVEL::STARTUP_CODES;
		break;


		case SETUP_LEVEL::STARTUP_CODES:
			delay(STARTUP_CODE_INPUT_TIME_MS);

			if(checkEnableStartupCodes()) {
				Serial.print(F("Enter Startup Codes: "));
				setStartupCodes(startupCodes);
				Serial.println(startupCodes);
			}

			setupState = SETUP_LEVEL::LCD;
		break;


		case SETUP_LEVEL::LCD:
			display = new Display;
			display->registerNewStartupCodes(startupCodeHandlers);
			coreComponents[CC_DISPLAY_INDEX] = display;
			setupState = SETUP_LEVEL::WELCOME;
		break;


		case SETUP_LEVEL::WELCOME:

			Serial.println();
			Serial.println(F("Welcome to glEEmail!"));
			Serial.print(F("Version "));
			Serial.println(GLEEMAIL_VERSION);
			Serial.println();

			display->updateReading("Hello, glEEmail!");
			delay(HELLO_GLEEMAIL_DELAY_MS);
			setupState = SETUP_LEVEL::STORAGE;
		break;


		case SETUP_LEVEL::STORAGE:
			storage = new Storage;
			if(!storage) {
				Serial.println(F("CRITICAL ALLOCATION FAILURE IN STORAGE SETUP"));
			}

			if(!storage->begin()) {
				DebugLog::getLog().logWarning(ERROR_CODE::STORAGE_NOT_DETECTED);
				delete storage;
				storage = nullptr;

				setupState = SETUP_LEVEL::NETWORKING;
			} else {
				storage->registerNewStartupCodes(startupCodeHandlers);
				coreComponents[CC_STORAGE_INDEX] = storage;
				setupState = SETUP_LEVEL::PREFERENCES;
			}
		break;


		case SETUP_LEVEL::PREFERENCES:
			if(preparePreferences(storage)) {
				initializeNetworkCredentialsFromPreferences(&desiredWiFiSSID, &desiredWiFiPassword);
			}

			setupState = SETUP_LEVEL::NETWORKING;
		break;


		case SETUP_LEVEL::NETWORKING:
			if(!network) {
				network = new Networking(&millis, &updateDisplayWithPeerChat, &connectedToPeerClearDisplay, 0, quit);
				network->registerNewStartupCodes(startupCodeHandlers);
			}

			Serial.println(F("Joining WiFi"));
			display->updateWriting("Joining WiFi");
			delay(SETUP_STEP_DELAY_MS);

			display->clearWriting();
			display->updateReading("Joining WiFi");
			delay(SETUP_STEP_DELAY_MS);

			networkCredentialsExist = desiredWiFiSSID && desiredWiFiPassword;
			if(!networkCredentialsExist || promptForNewNetworkCredentials) {
				networkCredentialsChanged = promptForNewWiFiCredentials(&desiredWiFiSSID, &desiredWiFiPassword, !networkCredentialsExist);
			}

			if(connectToWiFi(&internet, desiredWiFiSSID, desiredWiFiPassword)) {
				if(networkCredentialsChanged && storage) {
					Preferences::getPrefs().setWiFiSSID(desiredWiFiSSID);
					Preferences::getPrefs().setWiFiPassword(desiredWiFiPassword);

					const char* prefsData = Preferences::getPrefs().serializePrefs();
					if(!storage->writeFile(prefsData, Preferences::getPrefs().getPrefsPath())) {
						DebugLog::getLog().logError(ERROR_CODE::STORAGE_WRITE_UNEXPECTEDLY_FAILED);
					}
					delete[] prefsData;
				}

				coreComponents[CC_NETWORKING_INDEX] = network;
				setupState = SETUP_LEVEL::INPUT_METHOD;
			} else {
				promptForNewNetworkCredentials = true;
			}
		break;


		case SETUP_LEVEL::INPUT_METHOD:
			display->updateReading("Setting Up Input");
			if(!input) {
				input = new MorseCodeInput(LED_BUILTIN, &userMessageChanged, &sendChatMessage); //This assignment must be manually changed if a different input method is desired
			}

			if(setupInputMethod(&internet, storage)) {
				for(unsigned short i = 0; i < (MAX_MESSAGE_LENGTH + TERMINATOR); i += 1) {
					userMessage[i] = '\0';
					peerMessage[i] = '\0';
				}

				setupState = SETUP_LEVEL::PINS;
			} else {
				delay(NETWORK_FAILED_DELAY_MS); //Don't want to get blocked by GitHub when download is repeated
			}
		break;


		case SETUP_LEVEL::PINS:
			setupPins();
			setupState = SETUP_LEVEL::PEER;
		break;


		case SETUP_LEVEL::PEER:
			if(!OFFLINE_MODE) {
				display->clearWriting();
				display->updateReading("Enter glEEpal IP");
				connectToPeer(*network);
				display->clearWriting();
				display->updateReading("Wait for glEEpal");
			}

			setupState = SETUP_LEVEL::DONE;
		break;


		default:
			DebugLog::getLog().logError(ERROR_CODE::UNKNOWN_SETUP_STATE);
		break;
		}

		checkStartupCodes(startupCodes, startupCodeHandlers);
		printErrorCodes();
		delay(SETUP_STEP_DELAY_MS);
	} while (setupState != SETUP_LEVEL::DONE);
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

	bool quit = false;

	CoreComponent* coreComponents[COUNT_OF_CORE_COMPONENTS] = {nullptr};
	setup(quit, coreComponents);

	while(!quit) {
		cycleStartTime = millis();

		{ //InputMethod->Update();
			pinIndex = 0;

			Pin** allPins = input->getPins();
			Pin* currentPin = allPins[pinIndex];
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
		
		network->Update();

		{ //Display->Update();
			if(messageToPrint != nullptr) {
				display->updateReading(messageToPrint);
				delete[] messageToPrint;
				messageToPrint = nullptr;
			}
		}

		printErrorCodes();

#ifdef GLEEMAIL_FRAME_TIMER
		cycleDuration = millis() - cycleStartTime;
		if(cycleDuration > MAX_FRAME_DURATION_MS) {
			cycleLatencyCount += 1;
			if(cycleLatencyCount > FRAME_LATENCY_COUNT_ERROR_THRESHOLD) {
				DebugLog::getLog().logError(CONTINUOUS_FRAME_LATENCY);
				cycleLatencyCount = 0;
			}
		} else {
			cycleLatencyCount = 0;
		}
#endif

		if(arduino::serialEventRun) {
			arduino::serialEventRun();
		}
	}

	display->updateReading("glEEbye!");
	display->clearWriting();

	Serial.println(F("Lost connection to glEEpal"));
	if(arduino::serialEventRun) {
		arduino::serialEventRun();
	}

	delay(4000);
	display->clearAll();
	return 0;
}

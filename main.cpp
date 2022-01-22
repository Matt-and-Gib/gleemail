#define ARDUINO_MAIN
#include "Arduino.h"
#include "src/include/global.h"
#include "src/include/preferences.h"

#include "src/include/startupcodehandler.h"
#include "src/include/queue.h"
#include "src/include/keyvaluepair.h"

#include "src/include/display.h"
#include "src/include/storage.h"

#include "src/include/internetaccess.h"
#include "src/include/websiteaccess.h"

#include "src/include/networking.h"

#include "src/include/morsecode.h" //Manually change this if using different input method


#define GLEEMAIL_FRAME_TIMER


using namespace GLEEMAIL_DEBUG;


//Weak empty variant initialization function.
//May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() {}

extern USBDeviceClass USBDevice;

extern "C" void __libc_init_array(void);


const constexpr unsigned short SERIAL_READ_LOOP_DELAY_MS = 250;
const unsigned short MAX_STARTUP_CODES = 7;
const unsigned short STARTUP_CODE_PROCESSED = 127;

bool quit = false;

Display* display = nullptr;
Storage* storage = nullptr;

InternetAccess internet;
WebsiteAccess websiteAccess;

char* messageToPrint = nullptr;
void connectedToPeerClearDisplay();
void updateDisplayWithPeerChat(char*);
Networking network(&millis, &updateDisplayWithPeerChat, &connectedToPeerClearDisplay, 0, quit);

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
	display->updateWriting(chat);
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
		display->updateReading(messageToPrint);
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


inline void checkStartupCodes(char (& codes)[MAX_STARTUP_CODES + TERMINATOR], Queue<KVPair<char, StartupCodeHandlerData*>>& handlers) {
	//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("2"));} else {Serial.println(F("!2"));}

	for(unsigned short index = 0; index < MAX_STARTUP_CODES; index += 1) {
		//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("3"));} else {Serial.println(F("!3"));}
		if(codes[index] == STARTUP_CODE_PROCESSED) {
			//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("4"));} else {Serial.println(F("!4"));}
			continue;
		}
		//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("5"));} else {Serial.println(F("!5"));}

		unsigned short tempSearchIndex = 0;
		//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("6"));} else {Serial.println(F("!6"));}
		QueueNode<KVPair<char, StartupCodeHandlerData*>>* registeredHandler = handlers.peek();
		//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("7"));} else {Serial.println(F("!7"));}
		while(registeredHandler != nullptr) {
		//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("8"));} else {Serial.println(F("!8"));}
			tempSearchIndex += 1; //delete me!
			//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("9"));} else {Serial.println(F("!9"));}
			if(*(registeredHandler->getData()) == codes[index]) {
				//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("10"));} else {Serial.println(F("!10"));}
				Serial.print(F("Found handler for "));
				Serial.println(codes[index]);
				break;
			}
			//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("11"));} else {Serial.println(F("!11"));}

			registeredHandler = registeredHandler->getNode();
			//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("12"));} else {Serial.println(F("!12"));}
		}

		//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("13"));} else {Serial.println(F("!13"));}
		if(registeredHandler != nullptr) {
			//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("14"));} else {Serial.println(F("!14"));}
			Serial.print(F("Before handler call. Found handler on search: "));
			Serial.println(tempSearchIndex);

			KVPair<char, StartupCodeHandlerData*>* handlerPair = registeredHandler->getData();
			StartupCodeHandlerData* handlerValue = registeredHandler->getData()->getValue();
			const StartupCodeHandler* handlerInstance = registeredHandler->getData()->getValue()->instance;

			//if(handlers.peek()->getData()->getValue()->instance == nullptr) {Serial.println(F("15"));} else {Serial.println(F("!15"));}

			if(handlerPair == nullptr) {
				Serial.println(F("pair nullptr"));
			}
			
			if(handlerValue == nullptr) {
				Serial.println(F("value nullptr"));
			}

			if(handlerInstance == nullptr) {
				Serial.println(F("instance nullptr"));
			}

			//registeredHandler->getData()->getValue()->instance->startupCodeReceived(registeredHandler->getData()->getValue()->callback);
			registeredHandler->getData()->getValue()->instance->test_only_delete_me_asap();
			//codes[index] = STARTUP_CODE_PROCESSED; //DO NOT FORGET ABOUT THIS! UNCOMMEnT ME
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


bool preparePreferences() {
	if(!storage) {
		DebugLog::getLog().logError(ERROR_CODE::STORAGE_OBJECT_UNSUCCESSFULLY_ALLOCATED);
		return false;
	}

	const char* preferencesData = storage->readFile(Preferences::getPrefs().getPrefsPath());

	if(!preferencesData) {
		DebugLog::getLog().logWarning(ERROR_CODE::PREFERENCES_LOAD_FAILED);
		delete[] preferencesData;

		return false;
	} else {
		Preferences::getPrefs().deserializePrefs(preferencesData, storage->lastReadFileLength());
		delete[] preferencesData;

		return true;
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


bool connectToWiFi(char* desiredWiFiSSID, char* desiredWiFiPassword) {
	Serial.println(F("Attempting connection..."));
	display->updateWriting("Connecting...");

	if(!internet.connectToNetwork(desiredWiFiSSID, desiredWiFiPassword)) {
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
	display->updateWriting("Downloading Data");

	const char* rawVersionData = getDataFromInternet(input->getDataVersionRequestEndpoint());
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
		data = getDataFromInternet(input->getDataRequestEndpoint());
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
			storage->writeFile(data, input->getCachedDataPath());

			if(inputMethodDataVersion > Preferences::getPrefs().getMorseCodeCharPairsVersion()) {
				const char* prefsData = Preferences::getPrefs().serializePrefs();
				storage->writeFile(prefsData, Preferences::getPrefs().getPrefsPath());
				delete[] prefsData;
			}
		}
	}
	
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

	network.connectToPeer(friendsIP);

	Serial.print(F("Waiting for gleepal at "));
	Serial.println(friendsIP);

	delete[] ipAddressInputBuffer;
}


void setup() {
	enum class SETUP_LEVEL {BEGIN, SERIAL_COMM, STARTUP_CODES, LCD, WELCOME, STORAGE, PREFERENCES, NETWORK, INPUT_METHOD, PINS, PEER, DONE};
	SETUP_LEVEL setupState = SETUP_LEVEL::BEGIN;

	const unsigned short BAUD_RATE = 9600;

	char startupCodes[MAX_STARTUP_CODES + TERMINATOR] {0};
	Queue<KVPair<char, StartupCodeHandlerData*>> startupCodeHandlers;

	char* desiredWiFiSSID = nullptr;
	char* desiredWiFiPassword = nullptr;
	bool promptForNewNetworkCredentials = false;
	bool networkCredentialsChanged = false;
	bool networkCredentialsExist = false;

	const unsigned short SETUP_STEP_DELAY_MS = 100;
	const unsigned short STARTUP_CODE_INPUT_TIME_MS = 1500;
	const unsigned short HELLO_GLEEMAIL_DELAY_MS = 1000;
	const unsigned short NETWORK_FAILED_DELAY_MS = 3600; //Smallest GitHub rate limit is 1000/hour, and there are 3600000ms in one hour, therefore sending one request per 3600ms will hopefully ensure we dont' exceed any limits

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
			display->registerNewStartupCodes(startupCodeHandlers, display);

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

				setupState = SETUP_LEVEL::NETWORK;
			} else {
				storage->registerNewStartupCodes(startupCodeHandlers, storage);
				if(startupCodeHandlers.peek()->getData()->getValue() == nullptr) {
					Serial.println(F("MAIN: value in the queue already nullptr!"));
				} else {
					if(startupCodeHandlers.peek()->getData()->getValue()->instance == nullptr) {
						Serial.println(F("MAIN: instance from handler in queue is a nullptr despite the fact that we just created it. UGH"));
					} else {
						if(startupCodeHandlers.peek()->getData()->getValue()->instance == storage) {
							Serial.println(F("instance is == storage"));
						} else {
							Serial.println(F("instance != storage"));
						}

						Serial.println(F("MAIN: sanity check passed"));
						Serial.println(F("Check #1"));

						if(startupCodeHandlers.peek()->getData()->getValue()->instance == nullptr) {
							Serial.println(F("1"));
						} else {
							Serial.println(F("!1"));
						}
						checkStartupCodes(startupCodes, startupCodeHandlers);
						if(startupCodeHandlers.peek()->getData()->getValue()->instance == nullptr) {
							Serial.println(F("X"));
						} else {
							Serial.println(F("!X"));
						}
					}
				}

				Serial.println(F("check #2"));
				checkStartupCodes(startupCodes, startupCodeHandlers);
				
				setupState = SETUP_LEVEL::PREFERENCES;
			}
		break;


		case SETUP_LEVEL::PREFERENCES:
			if(preparePreferences()) {
				initializeNetworkCredentialsFromPreferences(&desiredWiFiSSID, &desiredWiFiPassword);
			}

			setupState = SETUP_LEVEL::NETWORK;
		break;


		case SETUP_LEVEL::NETWORK:
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

			if(connectToWiFi(desiredWiFiSSID, desiredWiFiPassword)) {
				if(networkCredentialsChanged && storage) {
					Preferences::getPrefs().setWiFiSSID(desiredWiFiSSID);
					Preferences::getPrefs().setWiFiPassword(desiredWiFiPassword);

					const char* prefsData = Preferences::getPrefs().serializePrefs();
					storage->writeFile(prefsData, Preferences::getPrefs().getPrefsPath());
					delete[] prefsData;
				}

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

			if(setupInputMethod()) {
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
				connectToPeer();
				display->clearWriting();
				display->updateReading("Wait for glEEpal");
			}

			setupState = SETUP_LEVEL::DONE;
		break;


		default:
			DebugLog::getLog().logError(ERROR_CODE::UNKNOWN_SETUP_STATE);
		break;
		}

		Serial.println(F("Going to check bottom of main now..."));
		if(&startupCodeHandlers == nullptr) {
			Serial.println(F("oops!"));
		}
		if(startupCodeHandlers.peek() == nullptr) {
			Serial.println(F("peek == nullptr"));
		} else {
			Serial.println(F("peek is not a nullptr"));
			if(startupCodeHandlers.peek()->getData() == nullptr) {
				Serial.println(F("MAIN 2: peek data nullptr"));
			} else {
				Serial.println(F("getData is not a nullptr"));
				if(startupCodeHandlers.peek()->getData()->getValue() == nullptr) {
					Serial.println(F("MAIN 2: value in the queue already nullptr!"));
				} else {
					Serial.println(F("getValue is not a nullptr"));
					if(startupCodeHandlers.peek()->getData()->getValue()->instance == nullptr) { //Accessing instance is a segfault here!
						Serial.println(F("MAIN 2: instance from handler in queue is a nullptr despite the fact that we just created it. UGH"));
					} else {
						Serial.println(F("MAIN 2: sanity check passed"));
					}
				}
			}
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

	delay(5000);
	display->clearAll();
	return 0;
}
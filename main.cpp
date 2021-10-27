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

#include "src/include/lib/LiteChaCha/keyinfrastructure.h"
#include "src/include/lib/LiteChaCha/authenticatedencrypt.h"

//Weak empty variant initialization function.
//May be redefined by variant files.
void initVariant() __attribute__((weak));
void initVariant() { }

extern USBDeviceClass USBDevice;

extern "C" void __libc_init_array(void);


void updateDisplayWithPeerChat(const char*);
void updateDisplayWithUserChat(const char*);

bool quit = false;


static Storage storage;
static Display display;

static InternetAccess internet;
static Networking network(&millis, &updateDisplayWithPeerChat, 0, quit);
static WebAccess webAccess;


static InputMethod* input;
static unsigned short pinIndex = 0;
static char* userMessage = new char[MAX_MESSAGE_LENGTH + 1];
static char* peerMessage = new char[MAX_MESSAGE_LENGTH + 1];
bool pendingPeerMessage = false;

/*static void noAsynchronousProcess() {}
void (*doAsynchronousProcess)() = &noAsynchronousProcess;
static unsigned short verifyInputMethodDataStep = 0;*/

static long long cycleStartTime = 0;
static long cycleDuration = 0;
//static long greatestCycleDuration = 0;
static unsigned short cycleLatencyCount = 0;


/*int bufferIndex = 0;
char* dataBuffer = new char[4096];
bool mccpDownloadStarted = false;
unsigned short mccpDownloadDataValue = 0;*/


/*void checkMorseCodeCharPairsDownloadComplete() {

}*/


//Commented out due at least in part to shared network buffer (i.e. cannot asychronously download MorseCodeCharPairs)
/*void verifyInputMethodData() {
	//if(internet.activeWebConnection()) {
		if(internet.responseAvailableFromWeb()) {
			mccpDownloadStarted = true;
			if(bufferIndex < 4096) {
				dataBuffer[bufferIndex++] = internet.nextCharInWebResponse();
			} else {
				DebugLog::getLog().logError(ERROR_CODE::WEB_ACCESS_DATA_BUFFER_OVERFLOW);
				return;
			}
		//}
	} else {
		if(mccpDownloadStarted) {
			dataBuffer[bufferIndex] = '\0';

			if(bufferIndex < 4096/2) {
				DebugLog::getLog().logWarning(ERROR_CODE::WEB_ACCESS_DATA_BUFFER_UNDERUTILIZED);
			}

			//Print full response
			for(int i = 0; i < bufferIndex; i += 1) {
				Serial.print(dataBuffer[i]);
			}
			Serial.println('\n');


			short endOfHeaderIndex = webAccess.findEndOfHeaderIndex(dataBuffer, bufferIndex);
			if(endOfHeaderIndex != -1) {
				const unsigned short LENGTH_OF_JSON_BODY = bufferIndex - endOfHeaderIndex;

				Serial.print(F("length of body: "));
				Serial.println(LENGTH_OF_JSON_BODY);

				char* payloadData = new char[LENGTH_OF_JSON_BODY];
				for(int i = 0; i < LENGTH_OF_JSON_BODY; i += 1) {
					payloadData[i] = dataBuffer[endOfHeaderIndex + i];
				}
				payloadData[LENGTH_OF_JSON_BODY] = '\0';

				mccpDownloadDataValue = atoi(payloadData);
				delete[] payloadData;
			}

			delete[] dataBuffer;


			Serial.print(F("v: "));
			Serial.println(mccpDownloadDataValue);

			if(mccpDownloadDataValue == Preferences::getPrefs().getMorseCodeCharPairsVersion()) {
				Serial.println(F("Versions match!"));
				doAsynchronousProcess = &noAsynchronousProcess;
			} else {
				Serial.println(F("Morse Code Char Pairs version mismatch!"));
				
				if(!webAccess.sendRequestToServer(internet, input->getServerAddress(), input->getRequestHeaders())) {
					//this is a huge problem! sendRequestToServer can fail only if WiFi is not connected, the SSL certificate for raw.githubusercontent.com is invalid, or if too few or too many headers are sent.
				}
				
				doAsynchronousProcess = &checkMorseCodeCharPairsDownloadComplete;
			}
		}
	}*/


	/*if(internet.activeWebConnection()) {
		if(internet.responseAvailableFromWeb()) {
			Serial.println(F("read byte"));
			mccpVersionInfoDownloadStarted = true;
			downloadBuffer[downloadBufferIndex++] = internet.nextCharInWebResponse();
		}
	} else {
		if(mccpVersionInfoDownloadStarted == true) {
			Serial.println(F("complete"));

			Serial.println(downloadBuffer);

			downloadBuffer[downloadBufferIndex] = '\0';

			short endOfHeaderIndex = webAccess.findEndOfHeaderIndex(downloadBuffer, downloadBufferIndex);
			if(endOfHeaderIndex != -1) {
				const unsigned short LENGTH_OF_JSON_BODY = downloadBufferIndex - endOfHeaderIndex;

				Serial.print(F("length of body: "));
				Serial.println(LENGTH_OF_JSON_BODY);

				char* payloadData = new char[LENGTH_OF_JSON_BODY];
				for(int i = 0; i < LENGTH_OF_JSON_BODY; i += 1) {
					payloadData[i] = downloadBuffer[endOfHeaderIndex + i];
				}
				payloadData[LENGTH_OF_JSON_BODY] = '\0';

				const unsigned short latestVersionNumber = atoi(payloadData);

				Serial.print(F("latest version number: "));
				Serial.println(latestVersionNumber);

				if(latestVersionNumber == Preferences::getPrefs().getMorseCodeCharPairsVersion()) {
					Serial.println(F("Versions match!"));
					doAsynchronousProcess = &noAsynchronousProcess;
				} else {
					Serial.println(F("Morse Code Char Pairs version mismatch!"));
					
					
					//if(!webAccess.sendRequestToServer(internet, input->getServerAddress(), input->getRequestHeaders())) {
						//this is a huge problem! sendRequestToServer can fail only if WiFi is not connected, the SSL certificate for raw.githubusercontent.com is invalid, or if too few or too many headers are sent.
					//}
					
					doAsynchronousProcess = &checkMorseCodeCharPairsDownloadComplete;
				}
			}
		} else {
			Serial.println(F("wait"));
		}
	}*/

	/*Serial.println(F("verifying..."));
	if(internet.responseAvailableFromWeb()) {
		Serial.println(F("web connection inactive"));

		char* rawValue = webAccess.downloadFromServer(internet);

		Serial.print(F("raw value: "));
		Serial.println(rawValue);

		const unsigned short latestVersionNumber = atoi(rawValue);

		delete[] rawValue;

		Serial.print(F("latest version number: "));
		Serial.println(latestVersionNumber);

		if(latestVersionNumber == Preferences::getPrefs().getMorseCodeCharPairsVersion()) {
			Serial.println(F("Versions match!"));
			doAsynchronousProcess = &noAsynchronousProcess;
		} else {
			Serial.println(F("Morse Code Char Pairs version mismatch!"));
			
			
			if(!webAccess.sendRequestToServer(internet, input->getServerAddress(), input->getRequestHeaders())) {
				//this is a huge problem! sendRequestToServer can fail only if WiFi is not connected, the SSL certificate for raw.githubusercontent.com is invalid, or if too few or too many headers are sent.
			}
			
			doAsynchronousProcess = &checkMorseCodeCharPairsDownloadComplete;
		}
	}*/
//}


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
	//Serial.println(messageBody);
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
		if(!(OFFLINE_MODE && (e == NETWORK_HEARTBEAT_FLATLINE))) {
			Serial.print(F("\nError Code: "));
			Serial.println(e);
		}

		e = DebugLog::getLog().getNextError();
	}
}


bool prepareStorage() {
	if(!storage.begin()) {
		return false;
	}

	const char* preferencesData = storage.readFile(prefsPath);
	if(!preferencesData) {
		DebugLog::getLog().logWarning(ERROR_CODE::STORAGE_COULDNT_LOAD_PREFS);
	} else {
		Preferences::getPrefs().deserializePrefs(preferencesData, storage.lastReadFileLength());
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
		const char* prefsData = Preferences::getPrefs().serializePrefs();
		storage.writeFile(prefsData, prefsPath, false/*CHANGE TO TRUE WHEN ENCRYPTION IS READY, YO*/);
		delete[] prefsData;
	}

	Serial.println(F("Connected!"));
	display.updateWriting("Connected!");
	return true;
}


const unsigned short getMorseCodeCharPairsVersion() {
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


bool setupInputMethod() {
	input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN, &userMessageChanged, &sendChatMessage);

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

//----------USED TO CLEAR THE SD CARD----------
/*
	storage.begin();
	if(!storage.clearFile(prefsPath)) {
		Serial.println(F("Unable to Prefs path"));
	}
	if(!storage.clearFile(morseCodeCharPairsPath)) {
		Serial.println(F("Unable to delete MorseCodeCharPairs path"));
	}

	Serial.println(F("Files deleted successfully. Halting"));
	abort();
*/
//----------USED TO CLEAR THE SD CARD----------

	enum SETUP_LEVEL : short {WELCOME = 0, STORAGE = 1, NETWORK = 2, INPUT_METHOD = 3, PINS = 4, PEER = 5, DONE = 6};
	SETUP_LEVEL setupState = WELCOME;
	bool setupComplete = false;
	bool networkForceNewCredentials = false;

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
				Serial.print(F("LocalIP: "));
				Serial.println(internet.getLocalIP());
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
		//updateDisplay();
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

	return 0;
}
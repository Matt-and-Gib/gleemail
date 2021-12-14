#include "include/morsecode.h"

#include "include/global.h"
#include "include/morsecodetree.h"

#include <ArduinoJson.h>


using namespace GLEEMAIL_DEBUG;


/*
	WARNING: STATISTICS BELOW:

	DOTS, DASHES, AND GAPS WERE MEASURED BY PROFESSIONALS IN A CLOSED TESTING ENVIRONMENT TO DETERMINE THE VALUES BELOW.
	THESE VALUES MAY NOT WORK FOR YOUR MEAGER MORSE TYPING SKILL LEVEL.
*/

namespace MORSE_CODE_LENGTHS {
	static const constexpr unsigned short CALCULATED_DOT_DURATION = 165;
	static const constexpr unsigned short DOT_DASH_THRESHOLD_BUFFER = 100;
	static const constexpr unsigned short DOT_DASH_THRESHOLD = CALCULATED_DOT_DURATION + DOT_DASH_THRESHOLD_BUFFER;

	static const constexpr unsigned short CALCULATED_DASH_DURATION = 2.35 * CALCULATED_DOT_DURATION;
	static const constexpr unsigned short MAX_DASH_THRESHOLD_BUFFER = 200;
	static const constexpr unsigned short MAX_DASH_THRESHOLD = CALCULATED_DASH_DURATION + MAX_DASH_THRESHOLD_BUFFER;

	static const constexpr unsigned short CALCULATED_PHRASE_FINISHED_THRESHOLD = 3 * CALCULATED_DOT_DURATION;
	static const constexpr unsigned short PHRASE_FINISHED_THRESHOLD_BUFFER = 100;
	static const constexpr unsigned short PHRASE_FINISHED_THRESHOLD = CALCULATED_PHRASE_FINISHED_THRESHOLD + PHRASE_FINISHED_THRESHOLD_BUFFER;

	static const constexpr unsigned short CALCULATED_WORD_FINISHED_THRESHOLD = 7 * CALCULATED_DOT_DURATION;
	static const constexpr unsigned short WORD_FINISHED_THRESHOLD_BUFFER = 400;
	static const constexpr unsigned short WORD_FINISHED_THRESHOLD = CALCULATED_WORD_FINISHED_THRESHOLD + WORD_FINISHED_THRESHOLD_BUFFER;

	static const constexpr unsigned short CALCULATED_MESSAGE_FINISHED_THRESHOLD = 3 * CALCULATED_WORD_FINISHED_THRESHOLD;
	static const constexpr unsigned short MESSAGE_FINISIHED_THRESHOLD_BUFFER = 500;
	static const constexpr unsigned short MESSAGE_FINISHED_THRESHOLD = CALCULATED_MESSAGE_FINISHED_THRESHOLD + MESSAGE_FINISIHED_THRESHOLD_BUFFER;
}
using namespace MORSE_CODE_LENGTHS;


static const constexpr char SERVER_REQUEST[] = "GET /Matt-and-Gib/gleemail/main/data/MorseCodeCharPairs.json HTTP/1.1";
static const constexpr short MCCP_REQUEST_HEADERS_LENGTH = 7;
static const constexpr char* REQUEST_HEADERS[MCCP_REQUEST_HEADERS_LENGTH] = {
	SERVER_REQUEST,
	NETWORK_HEADER_USER_AGENT,
	HOST,
	NETWORK_HEADER_ACCEPTED_RETURN_TYPE,
	NETWORK_HEADER_CONNECTION_LIFETIME,
	HEADER_TERMINATION,
	nullptr
};


MorseCodeInput::MorseCodeInput(const unsigned short switchPinLocation, const unsigned short ledPinLocation, void (*messageChanged)(char*), void (*sendMessage)(char*)) : InputMethod(messageChanged, sendMessage), currentMorsePhrase{*new MorsePhrase()}, morseCodeTreeRoot{*new MorseCodeTreeNode(*new MorsePhraseCharPair('\0', *new MorsePhrase()), nullptr)} {
	pins[0] = &NULL_PIN;
	pins[1] = &NULL_PIN;
	pins[2] = &NULL_PIN;

	Pin *switchDigitalPin = new Pin(switchPinLocation, PIN_MODE::READ, MORSE_CODE_STATE::SWITCH_OPEN);
	Pin *ledDigitalPin = new Pin(ledPinLocation, PIN_MODE::WRITE, MORSE_CODE_STATE::SWITCH_OPEN);

	pins[PINS_INDEX_SWITCH] = switchDigitalPin;
	pins[LED_PIN_INDEX] = ledDigitalPin;

	lastInputState = MORSE_CODE_STATE::SWITCH_OPEN;
	inputState = MORSE_CODE_STATE::SWITCH_OPEN;
}


MorseCodeInput::~MorseCodeInput() { //MEMORY LEAK
	//delete currentMorsePhrase;
}


unsigned short MorseCodeInput::setupInputMethod() {
	return 0;
}


const char* MorseCodeInput::getServerAddress() const {
	return SERVER;
}


const char* const* MorseCodeInput::getRequestHeaders() const {
	return REQUEST_HEADERS;
}


unsigned short MorseCodeInput::getDebounceThreshold() {
	return DEBOUNCE_THRESHOLD;
}


char MorseCodeInput::convertPhraseToCharacter() {
	MorsePhraseCharPair* lookupResult = morseCodeTreeRoot.lookup(currentMorsePhrase);
	if(!lookupResult) {
		DebugLog::getLog().logWarning(MORSE_CODE_LOOKUP_FAILED);
		return CANCEL_CHAR;
	}

	return lookupResult->character;
}


void MorseCodeInput::pushMorseCharacter(const MorseChar& morseCharacter) {
	if(currentMorsePhrase.phraseFull()) {
		DebugLog::getLog().logError(ERROR_CODE::MORSE_PHRASE_IMMINENT_OVERFLOW);
		pushCharacterToMessage(convertPhraseToCharacter());
		currentMorsePhrase.resetPhrase();
	}

	currentMorsePhrase.push(morseCharacter);
	if(currentMorsePhrase.phraseFull()) {
		pushCharacterToMessage(convertPhraseToCharacter());
		currentMorsePhrase.resetPhrase();
	}
}


//					***BUTTON RELEASED***
void MorseCodeInput::processClosedToOpen(const unsigned long currentCycleTime) {
	pins[PINS_INDEX_LED]->value = LED_STATUS::OFF;

	updateElapsedTime(currentCycleTime);

	if(elapsedCycleTime < DOT_DASH_THRESHOLD) {
		pushMorseCharacter(DOT);
	} else if (elapsedCycleTime < MAX_DASH_THRESHOLD) {
		pushMorseCharacter(DASH);
	}

	lastChangeTime = currentCycleTime;
	inputState = MORSE_CODE_STATE::SWITCH_OPEN;
}


//					***BUTTON PRESSED***
void MorseCodeInput::processOpenToClosed(const unsigned long currentCycleTime) {
	pins[PINS_INDEX_LED]->value = LED_STATUS::ON;

	lastChangeTime = currentCycleTime;
	inputState = MORSE_CODE_STATE::SWITCH_CLOSED;
}


void MorseCodeInput::updateElapsedTime(const unsigned long currentCycleTime) {
	if(lastChangeTime > currentCycleTime) {
		lastChangeTime = 0; //TODO: FIX ME
	}
	elapsedCycleTime = currentCycleTime - lastChangeTime;
}


void MorseCodeInput::checkPhraseElapsedThreshold() {
	if(currentMorsePhrase.phraseStarted()) {
		if(elapsedCycleTime >= PHRASE_FINISHED_THRESHOLD) {
			pushCharacterToMessage(convertPhraseToCharacter());
			currentMorsePhrase.resetPhrase();
		}
	}
}


void MorseCodeInput::checkMessageElapsedThresholds() {
	if(messageNotEmpty()) {
		if(elapsedCycleTime >= MESSAGE_FINISHED_THRESHOLD) {
			commitMessage();
		} else {
			if(!isLastCharSpace()) {
				if(elapsedCycleTime >= WORD_FINISHED_THRESHOLD) {
					pushCharacterToMessage(' ');
				}
			}
		}
	}
}


void MorseCodeInput::checkOpenElapsedTime(const unsigned long currentCycleTime) {
	updateElapsedTime(currentCycleTime);

	checkPhraseElapsedThreshold();
	checkMessageElapsedThresholds();
}


bool MorseCodeInput::setNetworkData(const char* payload) {
	if(!payload) {
		DebugLog::getLog().logError(JSON_NULLPTR_PAYLOAD);
		return false;
	}

	StaticJsonDocument<JSON_DOCUMENT_FILTER_FOR_SIZE_BYTES> filter;
	filter["size"] = true;

	StaticJsonDocument<JSON_DOCUMENT_FILTER_FOR_SIZE_BYTES> sizeDoc;
	deserializeJson(sizeDoc, payload, DeserializationOption::Filter(filter));
	const unsigned short mccpSize = sizeDoc["size"];

	DynamicJsonDocument mccpDoc(mccpSize);
	DeserializationError error = deserializeJson(mccpDoc, payload);

	if(error) {
		DebugLog::getLog().logError(ERROR_CODE::JSON_MORSECODE_NETWORK_DATA_DESERIALIZATION_ERROR);
		//Serial.println(error.f_str());
		return false;
	}

	const char* letter;
	const char* phrase;
	for (ArduinoJson::JsonObject elem : mccpDoc["morsecodetreedata"].as<ArduinoJson::JsonArray>()) {
		letter = elem["symbol"];
		phrase = elem["phrase"];
		morseCodeTreeRoot.insert(*new MorsePhraseCharPair(*letter, *new MorsePhrase(phrase)));
	}

	//morseCodeTreeRoot.print();
	return true;
}


void MorseCodeInput::processInput(const unsigned long currentCycleTime) {
	if(pins[PINS_INDEX_SWITCH]->value != lastInputState) {
		setLastDebounceTime(currentCycleTime);
	}

	if(currentCycleTime - getLastDebounceTime() > getDebounceThreshold()) {
		if(pins[PINS_INDEX_SWITCH]->value == MORSE_CODE_STATE::SWITCH_OPEN) {
			if(inputState == MORSE_CODE_STATE::SWITCH_CLOSED) {
				processClosedToOpen(currentCycleTime);
			}

			checkOpenElapsedTime(currentCycleTime);
		} else {
			if(inputState == MORSE_CODE_STATE::SWITCH_OPEN) {
				processOpenToClosed(currentCycleTime);
			}
		}
	}

	lastInputState = pins[PINS_INDEX_SWITCH]->value == 1 ? MORSE_CODE_STATE::SWITCH_CLOSED : MORSE_CODE_STATE::SWITCH_OPEN;
}
#include "include/morsecode.h"

#include "include/global.h"
#include "include/websiteaccess.h"


using namespace GLEEMAIL_DEBUG;


namespace {
	static const char DOT = '.';
	static const char DASH = '-';

	/*
		WARNING: STATISTICS BELOW:

		DOTS, DASHES, AND GAPS WERE MEASURED BY PROFESSIONALS IN A CLOSED TESTING ENVIRONMENT.
		THESE VALUES MAY NOT WORK FOR YOUR MEAGER MORSE SKILL LEVEL.
	*/

	const constexpr unsigned short CALCULATED_DOT_DURATION = 165;
	const constexpr unsigned short DOT_DASH_THRESHOLD_BUFFER = 100;
	const constexpr unsigned short DOT_DASH_THRESHOLD = CALCULATED_DOT_DURATION + DOT_DASH_THRESHOLD_BUFFER;

	const constexpr unsigned short CALCULATED_DASH_DURATION = 2.35 * CALCULATED_DOT_DURATION;
	const constexpr unsigned short MAX_DASH_THRESHOLD_BUFFER = 200;
	const constexpr unsigned short MAX_DASH_THRESHOLD = CALCULATED_DASH_DURATION + MAX_DASH_THRESHOLD_BUFFER;

	const constexpr unsigned short CALCULATED_PHRASE_FINISHED_THRESHOLD = 3 * CALCULATED_DOT_DURATION;
	const constexpr unsigned short PHRASE_FINISHED_THRESHOLD_BUFFER = 100;
	const constexpr unsigned short PHRASE_FINISHED_THRESHOLD = CALCULATED_PHRASE_FINISHED_THRESHOLD + PHRASE_FINISHED_THRESHOLD_BUFFER;

	const constexpr unsigned short CALCULATED_WORD_FINISHED_THRESHOLD = 7 * CALCULATED_DOT_DURATION;
	const constexpr unsigned short WORD_FINISHED_THRESHOLD_BUFFER = 400;
	const constexpr unsigned short WORD_FINISHED_THRESHOLD = CALCULATED_WORD_FINISHED_THRESHOLD + WORD_FINISHED_THRESHOLD_BUFFER;

	const constexpr unsigned short CALCULATED_MESSAGE_FINISHED_THRESHOLD = 3 * CALCULATED_WORD_FINISHED_THRESHOLD;
	const constexpr unsigned short MESSAGE_FINISIHED_THRESHOLD_BUFFER = 500;
	const constexpr unsigned short MESSAGE_FINISHED_THRESHOLD = CALCULATED_MESSAGE_FINISHED_THRESHOLD + MESSAGE_FINISIHED_THRESHOLD_BUFFER;
}


MorseCodeInput::MorseCodeInput(
	const unsigned short ledPinLocation,
	void (* const messageChanged)(char*),
	void (* const sendMessage)(char*)
) :
	InputMethod(messageChanged, sendMessage)
{
	pins[0] = &NULL_PIN;
	pins[1] = &NULL_PIN;
	pins[2] = &NULL_PIN;

	pins[PINS_INDEX_SWITCH] = new Pin(SWITCH_PIN_LOCATION, Pin::PIN_MODE::READ, MORSE_CODE_STATE::SWITCH_OPEN);
	pins[PINS_INDEX_LED] = new Pin(ledPinLocation, Pin::PIN_MODE::WRITE, MORSE_CODE_STATE::SWITCH_OPEN);

	currentInputState = (pins[PINS_INDEX_SWITCH]->value == MORSE_CODE_STATE::SWITCH_OPEN ? MORSE_CODE_STATE::SWITCH_OPEN : MORSE_CODE_STATE::SWITCH_CLOSED);
	lastInputState = currentInputState;
}


MorseCodeInput::~MorseCodeInput() {
	delete morsePhraseSymbols;

	delete pins[PINS_INDEX_LED];
	delete pins[PINS_INDEX_SWITCH];
}


unsigned short MorseCodeInput::filterJsonPayloadSize(const char* payload) const {
	StaticJsonDocument<JSON_DOCUMENT_FILTER_FOR_SIZE_BYTES> filter;
	filter["size"] = true;

	StaticJsonDocument<JSON_DOCUMENT_FILTER_FOR_SIZE_BYTES> sizeDoc;
	DeserializationError error = deserializeJson(sizeDoc, payload, DeserializationOption::Filter(filter));
	if(error) {
		DebugLog::getLog().logError(ERROR_CODE::JSON_MORSECODE_FILTER_DESERIALIZATION_ERROR);

		return 0;
	}

	return sizeDoc["size"];
}


unsigned short MorseCodeInput::calculateMorsePhraseSymbolsSize(const ArduinoJson::JsonArrayConst& rawMorseCodeTreeData) const { //This function ASSUMES that the ingested JSON document's last phrase's length is the longest used phrase length.
	ArduinoJson::JsonObjectConst lastElement = rawMorseCodeTreeData.getElement(rawMorseCodeTreeData.size() - 1);
	const char* const lastPhrase = lastElement["phrase"];

	unsigned short arraySize = 0;
	for(unsigned short index = strlen(lastPhrase); index > 0; index -= 1) {
		arraySize += (1 << index);
	}

	return arraySize;
}


unsigned short MorseCodeInput::calculateMorsePhraseIndex(const char* const phrase) const {
	unsigned char index = 0;
	unsigned char binaryPhrase = 0;
	while(phrase[index]) {
		if(phrase[index] == '.') {
			binaryPhrase <<= 1;
		} else {
			binaryPhrase = (binaryPhrase << 1) | 0b00000001;
		}

		index += 1;
	}

	binaryPhrase += (1 << index) - 2; //The bitshifted one is to represent powers of two, and the 2 is simply a constant offset due to indexing and the fact that 2^0 = 1.

	return binaryPhrase;
}


bool MorseCodeInput::setNetworkData(const char* payload) {
	if(!payload) {
		DebugLog::getLog().logError(MORSE_NULLPTR_PAYLOAD);
		return false;
	}

	DynamicJsonDocument mccpDoc = DynamicJsonDocument(filterJsonPayloadSize(payload)); //Dynamic because ArduinoJSON recommends using dynamic documents to store items greater than 1 kB.
	DeserializationError error = deserializeJson(mccpDoc, payload);
	if(error) {
		DebugLog::getLog().logError(ERROR_CODE::JSON_MORSECODE_NETWORK_DATA_DESERIALIZATION_ERROR);
		return false;
	} else if(mccpDoc.capacity() <= 0) {
		DebugLog::getLog().logError(ERROR_CODE::MORSE_FILTERED_JSON_PAYLOAD_SIZE_ZERO);
		return false;
	}

	ArduinoJson::JsonArrayConst rawMorseCodeTreeData = mccpDoc["morsecodetreedata"].as<ArduinoJson::JsonArrayConst>();

	morsePhraseSymbols = new char[calculateMorsePhraseSymbolsSize(rawMorseCodeTreeData)] {0};

	const char* symbol;
	for(ArduinoJson::JsonObjectConst elem : rawMorseCodeTreeData) {
		symbol = elem["symbol"];
		morsePhraseSymbols[calculateMorsePhraseIndex(elem["phrase"])] = *symbol;
	}

	return true;
}


void MorseCodeInput::resetMorsePhrase() {
	char* c = currentMorsePhrase;
	while(*c) {
		*c++ = 0;
	}
}


const char& MorseCodeInput::convertPhraseToCharacter() { //Make static
	const char& lookupResult = morsePhraseSymbols[calculateMorsePhraseIndex(currentMorsePhrase)];

	if(!lookupResult) {
		DebugLog::getLog().logWarning(MORSE_CODE_LOOKUP_FAILED);
		return CANCEL_CHAR;
	}

	return lookupResult;
}


void MorseCodeInput::pushMorseCharacter(const char& morseCharacter) {
	unsigned short index = 0;
	do {
		if(!currentMorsePhrase[index]) {
			currentMorsePhrase[index] = morseCharacter;
			if(index == 5) {
				pushCharacterToMessage(convertPhraseToCharacter());
				resetMorsePhrase();
			}
			return;
		}
	} while(index++ < 6);
}


//					***BUTTON RELEASED***
void MorseCodeInput::processClosedToOpen(const unsigned long& currentCycleTime) {
	pins[PINS_INDEX_LED]->value = LED_STATUS::OFF;

	updateElapsedTime(currentCycleTime);

	if(elapsedCycleTime < DOT_DASH_THRESHOLD) {
		pushMorseCharacter(DOT);
	} else if (elapsedCycleTime < MAX_DASH_THRESHOLD) {
		pushMorseCharacter(DASH);
	}

	lastStateChangeTime = currentCycleTime;
	currentInputState = MORSE_CODE_STATE::SWITCH_OPEN;
}


//					***BUTTON PRESSED***
void MorseCodeInput::processOpenToClosed(const unsigned long& currentCycleTime) {
	pins[PINS_INDEX_LED]->value = LED_STATUS::ON;

	lastStateChangeTime = currentCycleTime;
	currentInputState = MORSE_CODE_STATE::SWITCH_CLOSED;
}


void MorseCodeInput::updateElapsedTime(const unsigned long& currentCycleTime) {
	elapsedCycleTime = currentCycleTime - lastStateChangeTime; //Caution! After ~50 days of glEEmailing, currentCycleTime will overflow. Brief unintended behavior will result!
}


void MorseCodeInput::checkPhraseElapsedThreshold() {
	if(currentMorsePhrase[0] != 0) {
		if(elapsedCycleTime >= PHRASE_FINISHED_THRESHOLD) {
			pushCharacterToMessage(convertPhraseToCharacter());
			resetMorsePhrase();
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


void MorseCodeInput::checkOpenElapsedTime(const unsigned long& currentCycleTime) {
	updateElapsedTime(currentCycleTime);

	checkPhraseElapsedThreshold();
	checkMessageElapsedThresholds();
}


void MorseCodeInput::processInput(const unsigned long& currentCycleTime) {
	if(pins[PINS_INDEX_SWITCH]->value != lastInputState) {
		lastDebounceTime = currentCycleTime;
	}

	if(currentCycleTime - lastDebounceTime > DEBOUNCE_THRESHOLD) {
		if(pins[PINS_INDEX_SWITCH]->value == MORSE_CODE_STATE::SWITCH_OPEN) {
			if(currentInputState == MORSE_CODE_STATE::SWITCH_CLOSED) {
				processClosedToOpen(currentCycleTime);
			}

			checkOpenElapsedTime(currentCycleTime);
		} else {
			if(currentInputState == MORSE_CODE_STATE::SWITCH_OPEN) {
				processOpenToClosed(currentCycleTime);
			}
		}
	}

	lastInputState = pins[PINS_INDEX_SWITCH]->value == MORSE_CODE_STATE::SWITCH_CLOSED ? MORSE_CODE_STATE::SWITCH_CLOSED : MORSE_CODE_STATE::SWITCH_OPEN;
}


void MorseCodeInput::Update() {
	
}

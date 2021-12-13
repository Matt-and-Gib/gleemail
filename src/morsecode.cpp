#include "include/morsecode.h"


using namespace GLEEMAIL_DEBUG;


/*
	TODO:
[v]		implement basic morse char functions
[v]		implement morse char comparison
[v]		replace enum class usage with new type Morse Char
[v]		DEBOUNCE
[v]		finish BSTree data strcture
[v]		hardcode morse phrase to char pairs
[v]			push <currentMorsePhrase, char> pairs to tree
[v]		lookup on currentMorsePhrase (key) in BSTree
		...
*/


MorseCodeInput::MorseCodeInput(const unsigned short switchPinLocation, const unsigned short ledPinLocation, void (*messageChanged)(char*), void (*sendMessage)(char*)) : InputMethod(messageChanged, sendMessage) {
	Pin *switchDigitalPin = new Pin(switchPinLocation, PIN_MODE::READ, MORSE_CODE_STATE::SWITCH_OPEN);
	Pin *ledDigitalPin = new Pin(ledPinLocation, PIN_MODE::WRITE, MORSE_CODE_STATE::SWITCH_OPEN);

	pins[switchPinIndex] = switchDigitalPin;
	pins[ledPinIndex] = ledDigitalPin;
}


MorseCodeInput::~MorseCodeInput() { //MEMORY LEAK
	//delete currentMorsePhrase;
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
	pins[ledPinIndex]->value = LED_STATUS::OFF;

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
	pins[ledPinIndex]->value = LED_STATUS::ON;

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
		Serial.println(error.f_str());
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
	if(pins[switchPinIndex]->value != lastInputState) {
		setLastDebounceTime(currentCycleTime);
	}

	if(currentCycleTime - getLastDebounceTime() > getDebounceThreshold()) {
		if(pins[switchPinIndex]->value == MORSE_CODE_STATE::SWITCH_OPEN) {
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

	lastInputState = pins[switchPinIndex]->value == 1 ? MORSE_CODE_STATE::SWITCH_CLOSED : MORSE_CODE_STATE::SWITCH_OPEN;
}
#include "include/morsecode.h"


/*
	TODO:
[v]		implement basic morse char functions
[v]		implement morse char comparison
[v]		replace enum class usage with new type Morse Char
[v]		DEBOUNCE
[v]		finish BSTree data strcture
[v]		hardcode morse phrase to char pairs
[v]			push <MorsePhrase, char> pairs to tree
[v]		lookup on MorsePhrase (key) in BSTree
		...
*/


MorseCodeInput::MorseCodeInput(const unsigned short switchPinLocation, const unsigned short ledPinLocation) {
	Pin *switchDigitalPin = new Pin(switchPinLocation, PIN_MODE::READ, MORSE_CODE_STATE::SWITCH_OPEN);
	Pin *ledDigitalPin = new Pin(ledPinLocation, PIN_MODE::WRITE, MORSE_CODE_STATE::SWITCH_OPEN);

	pins[switchPinIndex] = switchDigitalPin;
	pins[ledPinIndex] = ledDigitalPin;

	morsePhrase = MorsePhrase();
}


MorseCodeInput::~MorseCodeInput() { //MEMORY LEAK
	//delete morsePhrase;
}	


char MorseCodeInput::convertPhraseToCharacter() const {
	/*TreeNode* convertedPhrase = rootNode.lookup(MorsePhrase);
	if(!convertedPhrase) {
		pushErrorCode(ERROR_CODE::INVALID_CHAR_CONVERSION);
		return '\0';
	}

	return convertedPhrase.value;*/

	return 'a';
}


void MorseCodeInput::pushMorseCharacter(const MorseChar* morseCharacter) {
	if(morsePhrase.phraseFull()) {
		pushErrorCode(ERROR_CODE::MORSE_PHRASE_IMMINENT_OVERFLOW);
		//pushCharacterToMessage(convertPhraseToCharacter());
		pushCharacterToMessage('~');
		morsePhrase.resetPhrase();
	}

	if(morseCharacter == nullptr) {
		pushErrorCode(ERROR_CODE::INPUT_MORSE_CHAR_NOTHING);
		return;
	}

	morsePhrase.push(morseCharacter);
	if(morsePhrase.phraseFull()) {
		pushCharacterToMessage(convertPhraseToCharacter());
		morsePhrase.resetPhrase();
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
	if(morsePhrase.phraseStarted()) {
		if(elapsedCycleTime >= PHRASE_FINISHED_THRESHOLD) {
			pushCharacterToMessage(convertPhraseToCharacter());
			morsePhrase.resetPhrase();
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


void MorseCodeInput::setNetworkData(const char* payload) {
	if(!payload) {
		return;
	}
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
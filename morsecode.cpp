#include "morsecode.h"


MorseCodeInput::MorseCodeInput(const unsigned short switchPinLocation, const unsigned short ledPinLocation) {
	Pin *switchDigitalPin = new Pin(switchPinLocation, PIN_MODE::READ, MORSE_CODE_STATE::OPEN);
	Pin *ledDigitalPin = new Pin(ledPinLocation, PIN_MODE::WRITE, MORSE_CODE_STATE::OPEN);

	pins[switchPinIndex] = switchDigitalPin;
	pins[ledPinIndex] = ledDigitalPin;

	morsePhrase = new MORSE_CHAR[MAX_MORSE_PHRASE_LENGTH];
	for(int i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
		morsePhrase[i] = MORSE_CHAR::NOTHING;
	}
}


MorseCodeInput::~MorseCodeInput() {
	delete[] morsePhrase;
}


char MorseCodeInput::convertPhraseToCharacter() const {
	char convertedChar = '\0';
	return static_cast<const char>(convertedChar);
}


void MorseCodeInput::pushMorseCharacter(const MORSE_CHAR character) {
	if(morsePhraseIndex >= MAX_MORSE_PHRASE_LENGTH) {
		pushErrorCode(ERROR_CODE::MORSE_PHRASE_IMMINENT_OVERFLOW);
		//pushCharToMessage(convertPhraseToCharacter())
		//reset phrase array
	}

	if(character == MORSE_CHAR::NOTHING) {
		pushErrorCode(ERROR_CODE::INPUT_MORSE_CHAR_NOTHING);
		return;
	}

	morsePhrase[morsePhraseIndex] = character;
	morsePhraseIndex += 1;

	if(morsePhraseIndex == MAX_MORSE_PHRASE_LENGTH) {
		pushCharacterToMessage(convertPhraseToCharacter());
		resetMorsePhrase();
	}
}


//					***BUTTON RELEASED***
void MorseCodeInput::processClosedToOpen(const unsigned long currentCycleTime) {
	pins[ledPinIndex]->value = LED_STATUS::OFF;

	/*if(elapsedCycleTime < MIN_DASH_THRESHOLD) {
		pushMorseCharacter(MORSE_CHAR::DOT);
	} else {
		pushMorseCharacter(MORSE_CHAR::DASH);
	}*/

	if(elapsedCycleTime < DOT_DASH_THRESHOLD) {
		pushMorseCharacter(MORSE_CHAR::DOT);
	} else if (elapsedCycleTime < MAX_DASH_THRESHOLD) {
		pushMorseCharacter(MORSE_CHAR::DASH);
	}

	lastChangeTime = currentCycleTime;
	inputState = MORSE_CODE_STATE::OPEN;
}


//					***BUTTON PRESSED***
void MorseCodeInput::processOpenToClosed(const unsigned long currentCycleTime) {
	pins[ledPinIndex]->value = LED_STATUS::ON;

	lastChangeTime = currentCycleTime;
	inputState = MORSE_CODE_STATE::CLOSED;
}


void MorseCodeInput::resetMorsePhrase() {
	morsePhraseStarted = false;
	for(int i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
		morsePhrase[i] = MORSE_CHAR::NOTHING;
	}

	morsePhraseIndex = 0;
}


/*
	elapsedTime < PhraseFinished : same phrase
	elapsedTime > PhrasedFinished && elapsedTime < MESSAGE_FINISHED : new phrase and insert space
	elpasedTime > MESSAGE_FINISHED : send message and reset all


	every cycle, check elapsed time because 
*/

void MorseCodeInput::checkPhraseElapsedThreshold() {
	if(morsePhraseStarted) {
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


void MorseCodeInput::checkElapsedTime(const unsigned long currentCycleTime) {
	/*if(lastChangeTime > currentCycleTime) {
		lastChangeTime = 0;
	}*/
	elapsedCycleTime = currentCycleTime - lastChangeTime;

	checkPhraseElapsedThreshold();
	checkMessageElapsedThresholds();
}


void MorseCodeInput::processInput(const unsigned long currentCycleTime) {
	checkElapsedTime(currentCycleTime);

	if(pins[switchPinIndex]->value == MORSE_CODE_STATE::OPEN) {
		if(inputState == MORSE_CODE_STATE::CLOSED) {
			processClosedToOpen(currentCycleTime);
		}
	} else {
		if(inputState == MORSE_CODE_STATE::OPEN) {
			processOpenToClosed(currentCycleTime);
		}
	}
}
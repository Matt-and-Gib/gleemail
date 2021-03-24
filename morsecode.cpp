#include "morsecode.h"


MorseCodeInput::MorseCodeInput(const unsigned short switchPinLocation, const unsigned short ledPinLocation) {
	Pin *switchDigitalPin = new Pin(switchPinLocation, PIN_MODE::READ, MORSE_CODE_STATE::OPEN);
	Pin *ledDigitalPin = new Pin(ledPinLocation, PIN_MODE::WRITE, MORSE_CODE_STATE::OPEN);

	pins[switchPinIndex] = switchDigitalPin;
	pins[ledPinIndex] = ledDigitalPin;
}


MorseCodeInput::~MorseCodeInput() {
}


char MorseCodeInput::convertPhraseToCharacter() const {
	char convertedChar = '\0';
	return static_cast<const char>(convertedChar);
}


void MorseCodeInput::pushMorseCharacter(const MORSE_CHAR character) {
	if(morsePhraseIndex >= MAX_MORSE_PHRASE_LENGTH) {
		//pushErrorCode(1);
		//reset phrase array
	}

	switch (character) {
	case MORSE_CHAR::DOT:
		break;

	case MORSE_CHAR::DASH:
		break;

	case MORSE_CHAR::NOTHING:
		break;

	default:
		break;
	}

	if(morsePhraseIndex == MAX_MORSE_PHRASE_LENGTH) {
		//convert phrase to ascii char
		//reset phrase array
	}
}


//					***BUTTON RELEASED***
void MorseCodeInput::processClosedToOpen(const unsigned long currentCycleTime) {
	pins[ledPinIndex]->value = LED_STATUS::OFF;

	elapsedTime = currentCycleTime - lastChangeTime; //Duration of switch being closed
	if(elapsedTime < MIN_DASH_THRESHOLD) {
		pushMorseCharacter(MORSE_CHAR::DOT);
	} else {
		pushMorseCharacter(MORSE_CHAR::DASH);
	}

	lastChangeTime = currentCycleTime;
	inputState = MORSE_CODE_STATE::OPEN;
}


//					***BUTTON PRESSED***
void MorseCodeInput::processOpenToClosed(const unsigned long currentCycleTime) {
	pins[ledPinIndex]->value = LED_STATUS::ON;
	morsePhraseStarted = true;

	lastChangeTime = currentCycleTime;

	//calc diff in currentCycleTime - lastCharTime
	//interpret diff into dots/dashes
}


void MorseCodeInput::resetMorsePhrase() {
	morsePhraseStarted = false;
	for(int i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
		morsePhrase[i] = MORSE_CHAR::NOTHING;
	}
}


/*
	elapsedTime < PhraseFinished : same phrase
	elapsedTime > PhrasedFinished && elapsedTime < MESSAGE_FINISHED : new phrase and insert space
	elpasedTime > MESSAGE_FINISHED : send message and reset all


	every cycle, check elapsed time because 
*/

void MorseCodeInput::checkPhraseElapsedThreshold(const unsigned long elapsedTime) {
	if(morsePhraseStarted) {
		if(elapsedTime >= PHRASE_FINISHED_THRESHOLD) {
			pushCharacterToMessage(convertPhraseToCharacter());
			resetMorsePhrase();
		}
	}
}

void MorseCodeInput::checkMessageElapsedThresholds(const unsigned long elspasedTime) {
	if(messageNotEmpty()) {
		if(elapsedTime >= MESSAGE_FINISHED_THRESHOLD) {
			commitMessage();
		} else {
			if(!isLastCharSpace()) {
				if(elapsedTime >= WORD_FINISHED_THRESHOLD) {
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
	elapsedTime = currentCycleTime - lastChangeTime;

	checkPhraseElapsedThreshold(elapsedTime);
	checkMessageElapsedThresholds(elapsedTime);
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
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
	pins[ledPinIndex]->value = 0;

	/*elapsedTime = currentCycleTime - lastChangeTime; //Duration of switch being closed
	if(elapsedTime < MIN_DASH_THRESHOLD) {
		pushCharacter(MORSE_CHAR::DOT);
	}

	lastChangeTime = currentCycleTime;
	inputState = MORSE_CODE_STATE::OPEN;*/
}


//					***BUTTON PRESSED***
void MorseCodeInput::processOpenToClosed(const unsigned long currentCycleTime) {
	pins[ledPinIndex]->value = 1;
	morseCharStarted = true;

	lastChangeTime = currentCycleTime;

	//calc diff in currentCycleTime - lastCharTime
	//interpret diff into dots/dashes
}


void MorseCodeInput::processInput(const unsigned long currentCycleTime) {
	if(lastChangeTime > currentCycleTime) { //Overflow ~every 70 days
		lastChangeTime = 0;
	} //This may not be the best place to process this

	if(morseCharStarted) {
		elapsedTime = currentCycleTime - lastChangeTime;
		if(elapsedTime >= PHRASE_FINISHED_THRESHOLD) {
			pushCharacterToMessage(convertPhraseToCharacter());

			if(elapsedTime >= WORD_FINISHED_THRESHOLD) {
				pushCharacterToMessage(' ');

				if(elapsedTime >= MESSAGE_FINISHED_THRESHOLD) {
					commitMessage();
				}
			}
		}
	}

	//Check morseCharStarted == true
		//Check WORD_FINISHED_THRESHOLD met/exceeded
			//Append space
			//Set flag false

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
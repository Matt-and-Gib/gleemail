#include "morsecode.h"


MorseCodeInput::MorseCodeInput(const unsigned short switchPinLocation, const unsigned short ledPinLocation) {
	Pin *switchDigitalPin = new Pin(switchPinLocation, PIN_MODE::READ, MORSE_CODE_STATE::OPEN);
	Pin *ledDigitalPin = new Pin(ledPinLocation, PIN_MODE::WRITE, MORSE_CODE_STATE::OPEN);

	pins[switchPinIndex] = switchDigitalPin;
	pins[ledPinIndex] = ledDigitalPin;
}


MorseCodeInput::~MorseCodeInput() {
}


void MorseCodeInput::pushCharacter(const MORSE_CHAR character) {
	switch (character) {
	case MORSE_CHAR::DOT:
		break;

	case MORSE_CHAR::DASH:
		break;

	case MORSE_CHAR::NOTHING:
		break;

	case default:
		break;
	}
}


void MorseCodeInput::processClosedToOpen(const unsigned long currentCycleTime) {
	pins[ledPinIndex]->value = 0;

	elapsedTime = currentCycleTime - lastChangeTime; //Duration of switch being closed
	if(elapsedTime < MIN_DASH_THRESHOLD) {
		pushCharacter(MORSE_CHAR::DOT);
	}

	lastChangeTime = currentCycleTime;
	inputState = MORSE_CODE_STATE::CLOSED;
}


void MorseCodeInput::processOpenToClosed(const unsigned long currentCycleTime) {
	pins[ledPinIndex]->value = 1;

	elapsedTime = currentCycleTime - lastChangeTime; //Duration of switch being open

	lastChangeTime = currentCycleTime;
	//calc diff in currentCycleTime - lastCharTime
	//interpret diff into dots/dashes
}


void MorseCodeInput::processInput(const unsigned long currentCycleTime) {
	if(lastChangeTime > currentCycleTime) {
		lastChangeTime = 0;
	} //This may not be the best place to process this

	if(pins[switchPinIndex]->value == MORSE_CODE_STATE::CLOSED) {
		if(inputState == MORSE_CODE_STATE::OPEN) {
			processOpenToClosed(currentCycleTime);
		}
	} else {
		if(inputState == MORSE_CODE_STATE::CLOSED) {
			processClosedToOpen(currentCycleTime);
		}
	}
}
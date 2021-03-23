#include "morsecode.h"


MorseCodeInput::MorseCodeInput(const unsigned short switchPinLocation, const unsigned short ledPinLocation) {
	Pin *switchDigitalPin = new Pin(switchPinLocation, PIN_MODE::READ, MORSE_CODE_INPUT::OPEN);
	Pin *ledDigitalPin = new Pin(ledPinLocation, PIN_MODE::WRITE, MORSE_CODE_INPUT::OPEN);

	pins[switchPinIndex] = switchDigitalPin;
	pins[ledPinIndex] = ledDigitalPin;
}


MorseCodeInput::~MorseCodeInput() {
}


void MorseCodeInput::processInput(const unsigned long time) {
	if(time == 0) {}

	switch(pins[switchPinIndex]->value) {
	case MORSE_CODE_INPUT::CLOSED:
		pins[ledPinIndex]->value = 100;
		break;

	case MORSE_CODE_INPUT::OPEN:
		pins[ledPinIndex]->value = 0;
		break;
	}
}
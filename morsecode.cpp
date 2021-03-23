#include "morsecode.h"


MorseCodeInput::MorseCodeInput(const unsigned short switchPinIndex, const unsigned short ledPinIndex) {
	switchDigitalPin = new Pin(switchPinIndex, PIN_MODE::READ, MORSE_CODE_INPUT::OPEN);
	ledDigitalPin = new Pin(ledPinIndex, PIN_MODE::WRITE, MORSE_CODE_INPUT::OPEN);

	pins[0] = switchDigitalPin;
	pins[1] = ledDigitalPin;

	readPins[0] = switchDigitalPin;
	writePins[0] = ledDigitalPin;
}


MorseCodeInput::~MorseCodeInput() {

}


void MorseCodeInput::processInput(const unsigned long time) {
	switch(switchDigitalPin->value) {
	case MORSE_CODE_INPUT::CLOSED:
		ledDigitalPin->value = 1;
		break;

	case MORSE_CODE_INPUT::OPEN:
		ledDigitalPin->value = 0;
		break;
	}
}
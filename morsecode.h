#ifndef MORSECODE_H
#define MORSECODE_H

#include "inputmethod.h"


enum MORSE_CODE_INPUT : bool {CLOSED = 1, OPEN = 0};


class MorseCodeInput : public InputMethod {
public:
	MorseCodeInput(const unsigned short, const unsigned short);
	~MorseCodeInput();

	Pin **getPins() {return pins;}
	Pin **getReadPins() {return readPins;}
	Pin **getWritePins() {return writePins;}
	void processInput(const unsigned long time);
private:
	static constexpr unsigned short MAX_MORSE_CODE_INPUT_LENGTH = 6;

	Pin *switchDigitalPin = NULL_PIN;
	Pin *ledDigitalPin = NULL_PIN;
	Pin *pins[3] = {switchDigitalPin, ledDigitalPin, NULL_PIN};
	Pin *readPins[2] = {switchDigitalPin, NULL_PIN};
	Pin *writePins[2] = {ledDigitalPin, NULL_PIN};

	static constexpr unsigned short DOT_THRESHOLD = 10;
	static constexpr unsigned short MIN_DASH_THRESHOLD = 3 * DOT_THRESHOLD; //length of time required to enter dash
	static constexpr unsigned short MAX_INTRACHARACTER_GAP = DOT_THRESHOLD; //maximum amount of time to wait before completing character
	static constexpr unsigned short MAX_INTERCHARACTER_GAP = 3 * MAX_INTRACHARACTER_GAP; //maximum amount of time to wait before completing word
	static constexpr unsigned short MIN_SPACE_DELAY = 7; //maximum amount of time to wait before entering a space

	char inputCharacterCode[MAX_MORSE_CODE_INPUT_LENGTH] = {'\0', '\0', '\0', '\0', '\0', '\0'};
	char *message = new char;

	MORSE_CODE_INPUT inputState = MORSE_CODE_INPUT::OPEN;
	short typingDelayState = -1;
	unsigned long long lastCharTime;
};

#endif
#ifndef MORSECODE_H
#define MORSECODE_H

#include "inputmethod.h"


enum class MORSE_CODE_INPUT : bool {DOWN = false, UP = true};


class MorseCodeInput : public InputMethod {
public:
	MorseCodeInput();
	~MorseCodeInput();

	const Pin *getPins() const {return pins;}

	//const Pin *getInputPins() {return inputPins;}
	//const Pin *getOutputPins() {return outputPins;}

	void pushCharacter(const bool down = false);
private:
	static constexpr unsigned short MAX_MORSE_CODE_INPUT_LENGTH = 6;

	const Pin SWITCH_DIGITAL_PIN = Pin(4, PIN_MODE::READ);
	const Pin LED_DIGITAL_PIN = Pin(5, PIN_MODE::WRITE);
	const Pin pins[3] = {SWITCH_DIGITAL_PIN, LED_DIGITAL_PIN, NULL_PIN};

	static constexpr unsigned short DOT_THRESHOLD = 10;
	static constexpr unsigned short MIN_DASH_THRESHOLD = 3 * DOT_THRESHOLD; //length of time required to enter dash
	static constexpr unsigned short MAX_INTRACHARACTER_GAP = DOT_THRESHOLD; //maximum amount of time to wait before completing character
	static constexpr unsigned short MAX_INTERCHARACTER_GAP = 3 * MAX_INTRACHARACTER_GAP; //maximum amount of time to wait before completing word
	static constexpr unsigned short MIN_SPACE_DELAY = 7; //maximum amount of time to wait before entering a space

	char inputCharacterCode[MAX_MORSE_CODE_INPUT_LENGTH] = {'\0', '\0', '\0', '\0', '\0', '\0'};
	char *message = new char;

	MORSE_CODE_INPUT inputState = MORSE_CODE_INPUT::UP;
	short typingDelayState = -1;
	unsigned long long lastCharTime;
};

#endif
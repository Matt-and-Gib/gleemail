#ifndef MORSECODE_H
#define MORSECODE_H

#include "Arduino.h"


enum class MORSE_CODE_INPUT : bool {DOWN = false, UP = true};


class MorseCodeInput {
public:
	MorseCodeInput();
	~MorseCodeInput();

	unsigned short getSwitchPin() {return SWITCH_DIGITAL_PIN;}
	unsigned short getLEDPin() {return LED_DIGITAL_PIN;}

	void pushCharacter(const bool down = false);
private:
	static constexpr unsigned short SWITCH_DIGITAL_PIN = 4;
	static constexpr unsigned short LED_DIGITAL_PIN = 5;

	static constexpr unsigned short DOT_THRESHOLD = 10;
	static constexpr unsigned short MIN_DASH_THRESHOLD = 3 * DOT_THRESHOLD; //length of time required to enter dash
	static constexpr unsigned short MAX_INTRACHARACTER_GAP = DOT_THRESHOLD; //maximum amount of time to wait before completing character
	static constexpr unsigned short MAX_INTERCHARACTER_GAP = 3 * MAX_INTRACHARACTER_GAP; //maximum amount of time to wait before completing word
	static constexpr unsigned short MIN_SPACE_DELAY = 7; //maximum amount of time to wait before entering a space

	char inputCharacterCode[6] = {'\0', '\0', '\0', '\0', '\0', '\0'};
	char *message = new char;

	MORSE_CODE_INPUT inputState = MORSE_CODE_INPUT::UP;
	short typingDelayState = -1;
	unsigned long long lastCharTime;
};

#endif
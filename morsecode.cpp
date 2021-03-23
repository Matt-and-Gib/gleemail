#include "morsecode.h"


MorseCodeInput::MorseCodeInput() {

}


MorseCodeInput::~MorseCodeInput() {

}


void MorseCodeInput::pushCharacter (const bool down) {
	if(down) {
		lastCharTime = millis();
	}
}
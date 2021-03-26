#include "morsecode.h"


/*
	TODO:
[v]		implement basic morse char functions
[v]		implement morse char comparison
[v]		replace enum class usage with new type Morse Char
		DEBOUNCE
		finish BSTree data strcutre
		hardcode morse phrase to char pairs
			push <MorsePhrase, char> pairs to tree
		lookup on MorsePhrase (key) in BSTree
		...
*/


MorsePhrase::MorsePhrase() {
	phraseArray = new MorseChar[MAX_MORSE_PHRASE_LENGTH]();
	firstOpenIndex = 0;
}


MorsePhrase::~MorsePhrase() {
	delete[] phraseArray;
}


MorseChar* MorsePhrase::operator[](short unsigned int index) {
	return &phraseArray[index];
}


bool MorsePhrase::operator==(MorsePhrase& o) {
	for(int i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
		if(phraseArray[i] != *o[i]) {
			return false;
		}

		if(phraseArray[i] == *NOTHING) {
			return true;
		}
	}

	return true;
}


bool MorsePhrase::push(const MorseChar* morseCharacterToAdd) {
	if(!phraseFull()) {
		phraseArray[firstOpenIndex++] = *morseCharacterToAdd;
		return true;
	}

	return false;
}


void MorsePhrase::resetPhrase() {
	for(int i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
		phraseArray[i] = *NOTHING;
	}

	firstOpenIndex = 0;
}


MorseCodeInput::MorseCodeInput(const unsigned short switchPinLocation, const unsigned short ledPinLocation) {
	Pin *switchDigitalPin = new Pin(switchPinLocation, PIN_MODE::READ, MORSE_CODE_STATE::OPEN);
	Pin *ledDigitalPin = new Pin(ledPinLocation, PIN_MODE::WRITE, MORSE_CODE_STATE::OPEN);

	pins[switchPinIndex] = switchDigitalPin;
	pins[ledPinIndex] = ledDigitalPin;

	morsePhrase = MorsePhrase();
}


MorseCodeInput::~MorseCodeInput() { //MEMORY LEAK
	//delete morsePhrase;
}


char MorseCodeInput::convertPhraseToCharacter() const {
	return 'a';
}


void MorseCodeInput::pushMorseCharacter(const MorseChar* morseCharacter) {
	if(morsePhrase.phraseFull()) {
		pushErrorCode(ERROR_CODE::MORSE_PHRASE_IMMINENT_OVERFLOW);
		//pushCharacterToMessage(convertPhraseToCharacter());
		pushCharacterToMessage('~');
		morsePhrase.resetPhrase();
	}

	if(morseCharacter == nullptr) {
		pushErrorCode(ERROR_CODE::INPUT_MORSE_CHAR_NOTHING);
		return;
	}

	morsePhrase.push(morseCharacter);
	if(morsePhrase.phraseFull()) {
		pushCharacterToMessage(convertPhraseToCharacter());
		morsePhrase.resetPhrase();
	}
}


//					***BUTTON RELEASED***
void MorseCodeInput::processClosedToOpen(const unsigned long currentCycleTime) {
	pins[ledPinIndex]->value = LED_STATUS::OFF;

	updateElapsedTime(currentCycleTime);

	if(elapsedCycleTime < DOT_DASH_THRESHOLD) {
		pushMorseCharacter(DOT);
	} else if (elapsedCycleTime < MAX_DASH_THRESHOLD) {
		pushMorseCharacter(DASH);
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


void MorseCodeInput::updateElapsedTime(const unsigned long currentCycleTime) {
	if(lastChangeTime > currentCycleTime) {
		lastChangeTime = 0; //TODO: FIX ME
	}
	elapsedCycleTime = currentCycleTime - lastChangeTime;
}


void MorseCodeInput::checkPhraseElapsedThreshold() {
	if(morsePhrase.phraseStarted()) {
		if(elapsedCycleTime >= PHRASE_FINISHED_THRESHOLD) {
			pushCharacterToMessage(convertPhraseToCharacter());
			morsePhrase.resetPhrase();
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


void MorseCodeInput::checkOpenElapsedTime(const unsigned long currentCycleTime) {
	updateElapsedTime(currentCycleTime);

	checkPhraseElapsedThreshold();
	checkMessageElapsedThresholds();
}


void MorseCodeInput::processInput(const unsigned long currentCycleTime) {
	if(pins[switchPinIndex]->value != lastInputState) {
		setLastDebounceTime(currentCycleTime);
	}

	if(currentCycleTime - getLastDebounceTime() > getDebounceThreshold()) {
		if(pins[switchPinIndex]->value == MORSE_CODE_STATE::OPEN) {
			if(inputState == MORSE_CODE_STATE::CLOSED) {
				processClosedToOpen(currentCycleTime);
			}

			checkOpenElapsedTime(currentCycleTime);
		} else {
			if(inputState == MORSE_CODE_STATE::OPEN) {
				processOpenToClosed(currentCycleTime);
			}
		}
	}

	lastInputState = pins[switchPinIndex]->value == 1 ? MORSE_CODE_STATE::CLOSED : MORSE_CODE_STATE::OPEN;
}
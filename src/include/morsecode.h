#ifndef MORSECODE_H
#define MORSECODE_H

#include "inputmethod.h"


class MorseChar;
class MorsePhrase;
class MorseCodeTreeNode;
enum MORSE_CODE_STATE : bool;
struct Pin;


//https://morsecode.world/international/morse2.html
//https://morsecode.world/international/timing.html

//https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1677-1-200910-I!!PDF-E.pdf


/*
	#Logic
	______

	• Input MORSE_CHAR::NOTHING : convert morsePhrase to char, reset morsePhrase
	• If period of time between opeing and closing swtich > PHRASE_FINISHED_THRESHOLD : convert moresePhrase to char, reset moresePhrase, input MORSE_CHAR at index 0
	• If period of time between opening and closing switch > WORD_FINISHED_THRESHOLD : add space to message

	• Message sent if timeout reached or 32 char limit reached
*/


static const constexpr unsigned short SWITCH_PIN_LOCATION = 9;


//static const constexpr unsigned short CALCULATED_MCCP_DOCUMENT_SIZE_IN_BYTES = 4096;


class MorseCodeInput : public InputMethod {
private:
	Pin* pins[3];
	const unsigned short PINS_INDEX_SWITCH = 0;
	const unsigned short PINS_INDEX_LED = 1;

	MorsePhrase& currentMorsePhrase;
	MorseCodeTreeNode& morseCodeTreeRoot;

	static const constexpr unsigned short DEBOUNCE_THRESHOLD = 25;

	MORSE_CODE_STATE lastInputState;
	MORSE_CODE_STATE inputState;
	short typingDelayState = -1;
	unsigned long lastChangeTime = 0;
	long long elapsedCycleTime = 0;
	void updateElapsedTime(const unsigned long);

	void processClosedToOpen(const unsigned long);
	void processOpenToClosed(const unsigned long);
	void pushMorseCharacter(const MorseChar&);
	char convertPhraseToCharacter();

	void checkOpenElapsedTime(const unsigned long);
	void checkPhraseElapsedThreshold();
	void checkMessageElapsedThresholds();
	void resetMorsePhrase();
public:
	MorseCodeInput(const unsigned short, const unsigned short, void (*)(char*), void (*)(char*));
	~MorseCodeInput();

	unsigned short setupInputMethod();

	bool setNetworkData(const char*);
	const char* getServerAddress() const;
	const char* const* getRequestHeaders() const;

	Pin** getPins() {return pins;}
	void processInput(const unsigned long);

	unsigned short getDebounceThreshold();
};

#endif
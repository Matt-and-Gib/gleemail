#ifndef MORSECODE_H
#define MORSECODE_H

#include "inputmethod.h"
#include "morsecodetree.h"


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


/*
	#Reminders
	__________

	• If switch depressed at launch, dot will be (immediately) pushed before switch is released
*/


/*
	WARNING: STATISTICS BELOW:

	DOTS, DASHES, AND GAPS WERE MEASURED BY PROFESSIONALS IN A CLOSED TESTING ENVIRONMENT TO DETERMINE THE VALUES BELOW.
	THESE VALUES MAY NOT WORK FOR YOUR MEAGER MORSE TYPING SKILL LEVEL.
*/

static constexpr unsigned short CALCULATED_DOT_DURATION = 165;
static constexpr unsigned short DOT_DASH_THRESHOLD_BUFFER = 100;
static constexpr unsigned short DOT_DASH_THRESHOLD = CALCULATED_DOT_DURATION + DOT_DASH_THRESHOLD_BUFFER;

static constexpr unsigned short CALCULATED_DASH_DURATION = 2.35 * CALCULATED_DOT_DURATION;
static constexpr unsigned short MAX_DASH_THRESHOLD_BUFFER = 200;
static constexpr unsigned short MAX_DASH_THRESHOLD = CALCULATED_DASH_DURATION + MAX_DASH_THRESHOLD_BUFFER;

static constexpr unsigned short CALCULATED_PHRASE_FINISHED_THRESHOLD = 3 * CALCULATED_DOT_DURATION;
static constexpr unsigned short PHRASE_FINISHED_THRESHOLD_BUFFER = 100;
static constexpr unsigned short PHRASE_FINISHED_THRESHOLD = CALCULATED_PHRASE_FINISHED_THRESHOLD + PHRASE_FINISHED_THRESHOLD_BUFFER;

static constexpr unsigned short CALCULATED_WORD_FINISHED_THRESHOLD = 7 * CALCULATED_DOT_DURATION;
static constexpr unsigned short WORD_FINISHED_THRESHOLD_BUFFER = 400;
static constexpr unsigned short WORD_FINISHED_THRESHOLD = CALCULATED_WORD_FINISHED_THRESHOLD + WORD_FINISHED_THRESHOLD_BUFFER;

static constexpr unsigned short CALCULATED_MESSAGE_FINISHED_THRESHOLD = 3 * CALCULATED_WORD_FINISHED_THRESHOLD;
static constexpr unsigned short MESSAGE_FINISIHED_THRESHOLD_BUFFER = 500;
static constexpr unsigned short MESSAGE_FINISHED_THRESHOLD = CALCULATED_MESSAGE_FINISHED_THRESHOLD + MESSAGE_FINISIHED_THRESHOLD_BUFFER;


static constexpr unsigned short SWITCH_PIN_INDEX = 9;


class MorseCodeInput : public InputMethod {
public:
	MorseCodeInput(const unsigned short, const unsigned short);
	~MorseCodeInput();

	unsigned short getDebounceThreshold() {return 25;}

	Pin **getPins() {return pins;}
	void processInput(const unsigned long);
private:
	const unsigned short switchPinIndex = 0;
	const unsigned short ledPinIndex = 1;
	Pin *pins[3] = {&NULL_PIN, &NULL_PIN, &NULL_PIN};

	MorsePhrase morsePhrase;

	MORSE_CODE_STATE lastInputState = MORSE_CODE_STATE::OPEN;
	MORSE_CODE_STATE inputState = MORSE_CODE_STATE::OPEN;
	short typingDelayState = -1;
	unsigned long lastChangeTime = 0;
	long long elapsedCycleTime = 0;
	void updateElapsedTime(const unsigned long);

	void processClosedToOpen(const unsigned long);
	void processOpenToClosed(const unsigned long);
	void pushMorseCharacter(const MorseChar*);
	char convertPhraseToCharacter() const;

	void checkOpenElapsedTime(const unsigned long);
	void checkPhraseElapsedThreshold();
	void checkMessageElapsedThresholds();
	void resetMorsePhrase();
};

#endif
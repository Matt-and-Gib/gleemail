#ifndef MORSECODE_H
#define MORSECODE_H

#include "inputmethod.h"

//https://morsecode.world/international/morse2.html
//https://morsecode.world/international/timing.html

/*
	#Definitions
	____________

	MORSE_CHAR : dot/dash/nothing

	MAX_MORSE_PHRASE_LENGTH: : maximum number of MORSE_CHARs that make up a Morse phrase
	morsePhrase : array of MORSE_CHAR

	message : string formed of chars
	morsePhraseStarted : true when input received, false after WORD_FINISHED_THRESHOLD exceeded
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

/*
static constexpr unsigned short DOT_THRESHOLD = 10;
static constexpr unsigned short MAX_DASH_THRESHOLD = 3 * DOT_THRESHOLD;
static constexpr unsigned short PHRASE_FINISHED_THRESHOLD = DOT_THRESHOLD; //max amount of time to wait to consider phrase finished
static constexpr unsigned short WORD_FINISHED_THRESHOLD = 3 * PHRASE_FINISHED_THRESHOLD; //max amount of time to wait to consider word finished
static constexpr unsigned short MESSAGE_FINISHED_THRESHOLD = 10 * WORD_FINISHED_THRESHOLD; //max amount of time to enter new words before message committed
*/

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


enum MORSE_CODE_STATE : bool {CLOSED = 1, OPEN = 0};
enum class MORSE_CHAR : char {NOTHING = '0', DOT = '1', DASH = '2'};

static constexpr unsigned short SWITCH_PIN_INDEX = 9;


class MorseCodeInput : public InputMethod {
public:
	MorseCodeInput(const unsigned short, const unsigned short);
	~MorseCodeInput();

	unsigned short getDebounceDelay() {return 25;}

	Pin **getPins() {return pins;}
	void processInput(const unsigned long);
private:
	const unsigned short switchPinIndex = 0;
	const unsigned short ledPinIndex = 1;
	Pin *pins[3] = {&NULL_PIN, &NULL_PIN, &NULL_PIN};

	static constexpr unsigned short MAX_MORSE_PHRASE_LENGTH = 6;
	MORSE_CHAR *morsePhrase;
	unsigned short morsePhraseIndex = 0;
	MORSE_CHAR inputCharacter = MORSE_CHAR::NOTHING;

	bool morsePhraseStarted = false;

	MORSE_CODE_STATE inputState = MORSE_CODE_STATE::OPEN;
	short typingDelayState = -1;
	unsigned long lastChangeTime = 0;
	long long elapsedCycleTime = 0;

	void processClosedToOpen(const unsigned long);
	void processOpenToClosed(const unsigned long);
	void pushMorseCharacter(const MORSE_CHAR);
	char convertPhraseToCharacter() const;

	void checkElapsedTime(const unsigned long);
	void checkPhraseElapsedThreshold();
	void checkMessageElapsedThresholds();
	void resetMorsePhrase();
};

#endif
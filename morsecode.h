#ifndef MORSECODE_H
#define MORSECODE_H

#include "inputmethod.h"
#include "binarysearchtree.h"


//https://morsecode.world/international/morse2.html
//https://morsecode.world/international/timing.html

//https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1677-1-200910-I!!PDF-E.pdf


enum MORSE_CODE_STATE : bool {CLOSED = 1, OPEN = 0};
enum class MORSE_CHAR_STATE : char {NOTHING = '0', DOT = '1', DASH = '2'};


class MorseChar {
public:
	MorseChar() {value = MORSE_CHAR_STATE::NOTHING;}
	MorseChar(const MORSE_CHAR_STATE v) {value = v;}

	bool operator==(const MorseChar& o) {return value == o.value;}
	bool operator!=(const MorseChar& o) {return value != o.value;}
	bool operator>(const MorseChar& o) {
		if(value == o.value) return false;
		if(o.value == MORSE_CHAR_STATE::NOTHING) return true;
		return (value == MORSE_CHAR_STATE::DASH);
	};
	bool operator<(const MorseChar& o) {
		if(value == o.value) return false;
		if(value == MORSE_CHAR_STATE::NOTHING) return true;
		return (value == MORSE_CHAR_STATE::DOT);
	};
protected:
	MORSE_CHAR_STATE value;
private:
};
static const MorseChar *DOT = new MorseChar(MORSE_CHAR_STATE::DOT);
static const MorseChar *DASH = new MorseChar(MORSE_CHAR_STATE::DASH);
static const MorseChar *NOTHING = new MorseChar(MORSE_CHAR_STATE::NOTHING);


class MorsePhrase {
public:
	MorsePhrase();
	~MorsePhrase();

	unsigned short getSize() const {return MAX_MORSE_PHRASE_LENGTH;}
	unsigned short getLength() const {return firstOpenIndex;}

	bool push(const MorseChar*);
	void resetPhrase();

	bool phraseStarted() const {return firstOpenIndex > 0;}
	bool phraseFull() const {return firstOpenIndex == MAX_MORSE_PHRASE_LENGTH;}

	MorseChar* operator[](short unsigned int);
	bool operator==(MorsePhrase&);
private:
	static constexpr unsigned short MAX_MORSE_PHRASE_LENGTH = 6;
	unsigned short firstOpenIndex;
	MorseChar *phraseArray;
};


struct MorsePhraseCharPair {
	char character;
	MorsePhrase morsePhrase;
};


class MorseCodeBSTNode : public BinarySearchTreeNode<MorseCodeCharPair> {
public:
	void sortSubtree();
private:
};


/*
list of characters in order from least (top) to greatest (bottom)

	5	• • • • •
	H	• • • •
	4	• • • • ━
	S	• • •
	V	• • • ━
	3	• • • ━ ━
	I	• •
	F	• • ━ •
	U	• • ━
	2	• • ━ ━ ━
	E	•
	L	• ━ • •
	R	• ━ •
	A	• ━
	P	• ━ ━ •
	W	• ━ ━
	J	• ━ ━ ━
	1	• ━ ━ ━ ━
	ROOT (char value of NULL, morse phrase of NULL)
	6	━ • • • •
	B	━ • • •
	D	━ • •
	X	━ • • ━
	N	━ •
	C	━ • ━ •
	K	━ • ━
	Y	━ • ━ ━
	T	━
	7	━ ━ • • •
	Z	━ ━ • •
	G	━ ━ •
	Q	━ ━ • ━
	M	━ ━
	8	━ ━ ━ • •
	O	━ ━ ━
	9	━ ━ ━ ━ •
	0	━ ━ ━ ━ ━

*/


/*

priority of each node is : number of MORESE_CHARs + val of each


	A	• ━
	B	━ • • •
	C	━ • ━ •
	D	━ • •
	E	•
	F	• • ━ •
	G	━ ━ •
	H	• • • •
	I	• •
	J	• ━ ━ ━
	K	━ • ━
	L	• ━ • •
	M	━ ━
	N	━ •
	O	━ ━ ━
	P	• ━ ━ •
	Q	━ ━ • ━
	R	• ━ •
	S	• • •
	T	━
	U	• • ━
	V	• • • ━
	W	• ━ ━
	X	━ • • ━
	Y	━ • ━ ━
	Z	━ ━ • •

	const unsigned short asciiCapitalAOffset = (int)'A';
	for(int i = 0; i < 27; i += 1) {
		add[i + offset];
	}

	0	━ ━ ━ ━ ━
	1	• ━ ━ ━ ━
	2	• • ━ ━ ━
	3	• • • ━ ━
	4	• • • • ━
	5	• • • • •
	6	━ • • • •
	7	━ ━ • • •
	8	━ ━ ━ • •
	9	━ ━ ━ ━ •

	const unsigned short asciiZeroOffset = (int)'0';
	for(int i = 0; i < 11; i += 1) {
		add[i + offset];
	}

*/


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
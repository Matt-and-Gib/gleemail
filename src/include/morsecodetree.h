#ifndef MORSECODETREE_H
#define MORSECODETREE_H

#include "binarysearchtree.h"


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

	&	• ━ • • •
	+	• ━ • ━ •
	=	━ • • • ━
	/	━ • • ━ •
	(	━ • ━ ━ •
	
*/


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
static const MorseChar* DOT = new MorseChar(MORSE_CHAR_STATE::DOT);
static const MorseChar* DASH = new MorseChar(MORSE_CHAR_STATE::DASH);
static const MorseChar* NOTHING = new MorseChar(MORSE_CHAR_STATE::NOTHING);


class MorsePhrase {
public:
	MorsePhrase() {
		phraseArray = new MorseChar[MAX_MORSE_PHRASE_LENGTH]();
		firstOpenIndex = 0;
	}
	~MorsePhrase() {delete[] phraseArray;}

	MorseChar* operator[](const unsigned short index) {return &phraseArray[index];}

	bool operator==(MorsePhrase& o) {
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

	bool operator<(MorsePhrase&o) {
		for(int i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
			if(o[i] == NOTHING) {
				if(phraseArray[i] == *DOT) {
					return true;
				} else {
					return false;
				}
			}

			if(phraseArray[i] != *o[i]) {
				if(phraseArray[i] == *DOT) {
					return true;
				}
			}
		}

		return false;
	}

	unsigned short getSize() const {return MAX_MORSE_PHRASE_LENGTH;}
	unsigned short getLength() const {return firstOpenIndex;}

	bool push(const MorseChar* morseCharacterToAdd) {
		if(!phraseFull()) {
			phraseArray[firstOpenIndex++] = *morseCharacterToAdd;
			return true;
		}

		return false;
	}
	void resetPhrase() {
		for(int i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
			phraseArray[i] = *NOTHING;
		}

		firstOpenIndex = 0;
	}

	bool phraseStarted() const {return firstOpenIndex > 0;}
	bool phraseFull() const {return firstOpenIndex == MAX_MORSE_PHRASE_LENGTH;}
private:
	static constexpr unsigned short MAX_MORSE_PHRASE_LENGTH = 6;
	unsigned short firstOpenIndex;
	MorseChar *phraseArray;
};


struct MorsePhraseCharPair {
	MorsePhraseCharPair(const char c, MorsePhrase& p) {character = c; morsePhrase = p;}

	bool operator==(MorsePhraseCharPair& o) {return morsePhrase == o.morsePhrase;}
	bool isLessThan(unsigned short depth) {return morsePhrase[depth] == DOT ? true : false;}

	char character;
	MorsePhrase morsePhrase;
};


class MorseCodeTreeNode : public BinarySearchTreeNode<MorsePhraseCharPair> {
public:
	MorseCodeTreeNode(MorsePhraseCharPair& newData, MorseCodeTreeNode* newParent) {
		data = &newData;
		parentNode = newParent;
		lesserNode = nullptr;
		greaterNode = nullptr;
	}

	MorseCodeTreeNode* insert(MorsePhraseCharPair& dataToInsert, unsigned short depth = 0) {
		if (dataToInsert == *data) {
			return nullptr; //No duplicates allowed!
		}

		if(dataToInsert.isLessThan(depth)) {
			if(lesserNode == nullptr) {
				lesserNode = new MorseCodeTreeNode(dataToInsert, this);
				return lesserNode;
			} else {
				return lesserNode->insert(dataToInsert, depth + 1);
			}
		} else {
			if(greaterNode == nullptr) {
				greaterNode = new MorseCodeTreeNode(dataToInsert, this);
				return greaterNode;
			} else {
				return greaterNode->insert(dataToInsert, depth + 1);
			}
		}
	}

	MorsePhraseCharPair* lookup(MorsePhrase& phraseToConvert) {
		if(phraseToConvert[0] == NOTHING) {
			return nullptr;
		}

		if(phraseToConvert == data->morsePhrase) {
			return data;
		}

		if(phraseToConvert < data->morsePhrase) {
			return lesserNode->lookup(phraseToConvert);
		} else {
			return greaterNode->lookup(phraseToConvert);
		}
	}

	void print();
protected:
	MorseCodeTreeNode* parentNode;
	MorseCodeTreeNode* lesserNode;
	MorseCodeTreeNode* greaterNode;
private:
	void printSubtree(const short, const MorseCodeTreeNode*, bool);
};

#endif
#ifndef MORSECODETREE_H
#define MORSECODETREE_H

#include "binarysearchtree.h"


/*
	#Definitions 👺
	____________

	binaryPhrase : array of MORSE_CHAR
	MAX_MORSE_PHRASE_LENGTH: : maximum number of MORSE_CHARs that make up a Morse phrase

	message : string formed of chars
	morsePhraseStarted : true when input received, false after WORD_FINISHED_THRESHOLD exceeded
*/


/* *	KILL ME		* */
{"phrase": "---...", "symbol": ":"}
[DOT][DASH][...]
0b00000000

111000
111111 = 63
"111111"

char goboValueOfPhrase = 0b00111000;
char lengthOfPhrase = 6;
/* *	KILL ME		* */


namespace GLEEMAIL_MORSE_CODE {
	class MorsePhraseCharPair {
	public:
		char binaryPhrase = 0b00000000;
		char symbol;

		MorsePhraseCharPair(const char*, const char*);

		bool operator==(const MorsePhraseCharPair& rhs) const {
			//todo
		}

		bool operator<(const MorsePhraseCharPair& rhs) const {

		}
	};


	MorsePhraseCharPair::MorsePhraseCharPair(const char* letter, const char* phrase) {
		symbol = letter;

		char c = phrase[0];
		while(*c != '\0') {
			
			
			c++;
		}
	}


	class MorseCodeTreeNode final : public BinarySearchTreeNode<MorsePhraseCharPair> {
	private:
		BinarySearchTreeNode* addNode(BinarySearchTreeNode*) = delete;
	public:
		BinarySearchTreeNode* addNode(const char*, const char*);
	};


	BinarySearchTreeNode<MorsePhraseCharPair>* MorseCodeTreeNode::addNode(const char* letter, const char* phrase) {
		if(!letter || !phrase) {
			return nullptr;
		}

		MorsePhraseCharPair* pairToAdd = new MorsePhraseCharPair(letter, phrase);

	}
}

#endif
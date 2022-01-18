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


/*			KILL ME
{"phrase": "---...", "symbol": ":"}

"---..." -> 111000 -> 0b00111000
		Little Endian!		
		  0b00000111

Phrase length of 6, depth of 6. Root has a depth of 0.
			KILL ME			*/


namespace GLEEMAIL_MORSE_CODE {
	class MorsePhraseCharPair {
	public:
		char binaryPhrase = 0b00000000; //We could make this equal to 0b10000000 and then overwrite the most significant bit with a 0 when constructing as a sort of safety check.
		const char symbol;

		MorsePhraseCharPair(const char*, const char*, const char);

		bool operator==(const MorsePhraseCharPair& rhs) const {
			//todo
		}

		bool operator<(const MorsePhraseCharPair& rhs) const {
			//todo
		}
	};


	MorsePhraseCharPair::MorsePhraseCharPair(const char* letter, const char* phrase, const char length) : symbol{letter[0]} { //Added length parameter because it makes the conversion to binary easier. Not necessary!
		binaryPhrase = 0b00000000; //Is this redundant? Really need to make sure it is initialized as 0!

		for(char i = 0; i < length; i += 1) {
			if(phrase[(length - 1) - i] == '.') {
				binaryPhrase <<= 1; //Magic number simply represents one bit shift.
			} else { //fragile else statement.
				binaryPhrase = (binaryPhrase << 1) | 0b00000001; //Magic number simply represents the ones bit.
			}
		}
	}


	class MorseCodeTreeNode final : public BinarySearchTreeNode<MorsePhraseCharPair> {
	private:
		char calculatePhraseLength(const char*);
		//BinarySearchTreeNode* addNode(BinarySearchTreeNode*) = delete;
	public:
		MorseCodeTreeNode(const MorsePhraseCharPair& d) : BinarySearchTreeNode(d) {}
		BinarySearchTreeNode* addNode(const char*, const char*);
	};


	char MorseCodeTreeNode::calculatePhraseLength(const char* phrase) {
		char i = 0;

		while(phrase[i] != '\0') { //This could lead to problems if there is no null terminator! a.k.a it is fragile.
			i += 1;
		}

		return i;
	}


	BinarySearchTreeNode<MorsePhraseCharPair>* MorseCodeTreeNode::addNode(const char* letter, const char* phrase) {
		if(!letter || !phrase) {
			return nullptr;
		}

		char phraseLength = calculatePhraseLength(phrase); //This will need to be calculated eventually. May as well do it here as it speeds future processes up. Can be moved or even removed.

		MorsePhraseCharPair* newPair = new MorsePhraseCharPair(letter, phrase, phraseLength); //We need to know the length of the phrase after this point!
		BinarySearchTreeNode* newNode = new MorseCodeTreeNode(*newPair);

		BinarySearchTreeNode* currentNode = this;
		char depth = 0;
		while(currentNode != nullptr) {
			if((((newNode->getData().binaryPhrase) >> depth) & 0b00000001) == 0b00000000) { //This is where the realization that we really never compare anything becomes handy. '.' means left, '-' means right.
				if(currentNode->getLesserChild()) {
					currentNode = currentNode->getLesserChild();
					depth += 1;
				} else {
					currentNode->setLesserChild(*newNode);
					return newNode;
				}
			} else {
				if(currentNode->getGreaterChild()) {
					currentNode = currentNode->getGreaterChild();
					depth += 1;
				} else {
					currentNode->setGreaterChild(*newNode);
					return newNode;
				}
			}
		}
	}
}

#endif
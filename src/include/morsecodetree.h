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
	private:
		static constexpr unsigned short MAX_MORSE_PHRASE_LENGTH = 6; //This number should be equal to the longest supported morse phrase for your . Could be a maximum of 8 before we would need to switch data types.

		char binaryPhrase = 0b00000000; //A binary representation of a morse phrase, in little endian.
		const char symbol;
	public:
		explicit MorsePhraseCharPair(const char*, const char*);

		char getBinaryPhrase() {return binaryPhrase;}
		char getSymbol() {return symbol;}

		bool operator==(const MorsePhraseCharPair& rhs) const {
			//todo
		}

		bool operator<(const MorsePhraseCharPair& rhs) const {
			//todo
		}
	};


	MorsePhraseCharPair::MorsePhraseCharPair(const char* letter, const char* phrase) : symbol{letter[0]} {
		char length = 0;
		while((phrase[length] != '\0') && (length < MAX_MORSE_PHRASE_LENGTH)) {
			length += 1;
		}

		binaryPhrase = 0b00000000; //Redundant, but safe.
		for(char i = 0; i < length; i += 1) {
			if(phrase[(length - 1) - i] == '.') {
				binaryPhrase <<= 1;
			} else {
				binaryPhrase = (binaryPhrase << 1) | 0b00000001;
			}
		}
	}


	class MorseCodeTreeNode final : public BinarySearchTreeNode<MorsePhraseCharPair> {
	public:
		explicit MorseCodeTreeNode(const MorsePhraseCharPair& d) : BinarySearchTreeNode(d) {}
		~MorseCodeTreeNode() = default;
		BinarySearchTreeNode* addNode(const char*, const char*);
		BinarySearchTreeNode* addNode(BinarySearchTreeNode*) override;
	};


	BinarySearchTreeNode<MorsePhraseCharPair>* MorseCodeTreeNode::addNode(BinarySearchTreeNode* newNode) { //This is where it is useful to recognize that we are never comparing anything. '.' means go left, '-' means go right.
		BinarySearchTreeNode* currentNode = this;
		char depth = 0;
		while(currentNode != nullptr) {
			if(((newNode->getData()->getBinaryPhrase()) >> depth) & 0b00000001) { //Todo: if(!...) {getLesserChild()...}
				if(currentNode->getGreaterChild()) {
					currentNode = currentNode->getGreaterChild();
					depth += 1;
				} else {
					currentNode->setGreaterChild(*newNode);
					return newNode;
				}
			} else {
				if(currentNode->getLesserChild()) {
					currentNode = currentNode->getLesserChild();
					depth += 1;
				} else {
					currentNode->setLesserChild(*newNode);
					return newNode;
				}
			}
		}

		delete newNode;
		return nullptr;
	}


	BinarySearchTreeNode<MorsePhraseCharPair>* MorseCodeTreeNode::addNode(const char* letter, const char* phrase) { //This function requires that nodes be added in a certain order!
		if(!letter || !phrase) {
			return nullptr;
		}

		return addNode(new MorseCodeTreeNode(*new MorsePhraseCharPair(letter, phrase)));
	}
}

#endif
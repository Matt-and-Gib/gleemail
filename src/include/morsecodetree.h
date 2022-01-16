#ifndef MORSECODETREE_H
#define MORSECODETREE_H

#ifdef UNIT_TEST_MODE
//include moc_binarysearchtree.h
#else
#include "binarysearchtree.h"
#endif


/*
	#Definitions
	____________

	MORSE_CHAR : dot/dash/nothing

	morsePhrase : array of MORSE_CHAR
	MAX_MORSE_PHRASE_LENGTH: : maximum number of MORSE_CHARs that make up a Morse phrase

	message : string formed of chars
	morsePhraseStarted : true when input received, false after WORD_FINISHED_THRESHOLD exceeded
*/

/*
{"phrase": "---...", "symbol": ":"}
111000
111111 = 63
"111111"

char goboValueOfPhrase = 0b00111000;
char lengthOfPhrase = 6;
*/

namespace GLEEMAIL_MORSE_CODE {
	enum MORSE_CODE_STATE : bool {SWITCH_CLOSED = 1, SWITCH_OPEN = 0};
	enum class MORSE_CHAR_STATE : char {NOTHING = '0', DOT = '1', DASH = '2'};


	class MorseChar {
	protected:
		MORSE_CHAR_STATE value;
	public:
		explicit MorseChar() {value = MORSE_CHAR_STATE::NOTHING;}
		explicit MorseChar(const MORSE_CHAR_STATE v) {value = v;}

		bool operator==(const MorseChar& o) const {return value == o.value;}
		bool operator!=(const MorseChar& o) const {return value != o.value;}
		bool operator>(const MorseChar& o) const {
			if(value == o.value) return false;
			if(o.value == MORSE_CHAR_STATE::NOTHING) return true;
			return (value == MORSE_CHAR_STATE::DASH);
		};
		bool operator<(const MorseChar& o) const {
			if(value == o.value) return false;
			if(value == MORSE_CHAR_STATE::NOTHING) return true;
			return (value == MORSE_CHAR_STATE::DOT);
		};

		const char* toString() const {
			if(value == MORSE_CHAR_STATE::DOT) {
				return "DOT";
			} else if(value == MORSE_CHAR_STATE::DASH) {
				return "DASH";
			} else {
				return "NOTHING";
			}
		}
	};

	const MorseChar& DOT = *new MorseChar(MORSE_CHAR_STATE::DOT); //MEMORY LEAK
	const MorseChar& DASH = *new MorseChar(MORSE_CHAR_STATE::DASH); //MEMORY LEAK
	const MorseChar& NOTHING = *new MorseChar(MORSE_CHAR_STATE::NOTHING); //MEMORY LEAK


	class MorsePhrase {
	private:
		static constexpr unsigned short MAX_MORSE_PHRASE_LENGTH = 6;
		unsigned short firstOpenIndex;
		MorseChar* phraseArray;
	public:
		MorsePhrase() {
			phraseArray = new MorseChar[MAX_MORSE_PHRASE_LENGTH];
			/*for(unsigned short i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
				phraseArray[i] = NOTHING;
			}*/

			firstOpenIndex = 0;
		}

		MorsePhrase(const char* phrase) {
			phraseArray = new MorseChar[MAX_MORSE_PHRASE_LENGTH]();
			for(unsigned short i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
				if(phrase[i] == '\0') {
					for(unsigned short j = i; j < MAX_MORSE_PHRASE_LENGTH; j += 1) {
						phraseArray[j] = NOTHING;
					}
					break;
				} else {
					if(phrase[i] == '-') {
						phraseArray[i] = DASH;
					} else {
						phraseArray[i] = DOT;
					}
				}
			}
		}

		~MorsePhrase() {delete[] phraseArray;}

		bool operator==(const MorsePhrase& o) const {
			for(unsigned short i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
				if(phraseArray[i] != o.phraseArray[i]) {
					return false;
				}

				if(phraseArray[i] == NOTHING) {
					return true;
				}
			}

			return true;
		}

		bool operator<(const MorsePhrase&o) const {
			for(unsigned short i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
				if(o.phraseArray[i] == NOTHING) {
					if(phraseArray[i] == DOT) {
						return true;
					} else {
						return false;
					}
				}

				if(phraseArray[i] != o.phraseArray[i]) {
					if(phraseArray[i] == DOT) {
						return true;
					}
				}
			}

			return false;
		}

		unsigned short getSize() const {return MAX_MORSE_PHRASE_LENGTH;}
		unsigned short getLength() const {return firstOpenIndex;}

		const MorseChar& readAt(const unsigned short index) const {
			if(index < firstOpenIndex) {
				return phraseArray[index];
			} else {
				return NOTHING;
			}
		}

		bool push(const MorseChar& morseCharacterToAdd) {
			if(!phraseFull()) {
				phraseArray[firstOpenIndex++] = morseCharacterToAdd;
				return true;
			}

			return false;
		}
		void resetPhrase() {
			for(unsigned short i = 0; i < MAX_MORSE_PHRASE_LENGTH; i += 1) {
				phraseArray[i] = NOTHING;
			}

			firstOpenIndex = 0;
		}

		bool phraseStarted() const {return firstOpenIndex > 0;}
		bool phraseFull() const {return firstOpenIndex == MAX_MORSE_PHRASE_LENGTH;}
	};


	struct MorsePhraseCharPair {
		MorsePhraseCharPair(const char c, MorsePhrase& p) {
			character = c;
			morsePhrase = p;
		}

		bool operator==(const MorsePhraseCharPair& o) const {return morsePhrase == o.morsePhrase;}
		bool isLessThan(const unsigned short depth) const {return morsePhrase.readAt(depth) == DOT ? true : false;}
		bool operator<(const MorsePhraseCharPair& o) const {
			if(morsePhrase.getLength() == o.morsePhrase.getLength()) {
				for(unsigned short i = 0; i < o.morsePhrase.getLength(); i += 1) {
					if(morsePhrase.readAt(i) == DOT && o.morsePhrase.readAt(i) != DOT) {
						return true;
					}
				}
			} else {
				return morsePhrase.getLength() < o.morsePhrase.getLength();
			}
		}

		char character;
		MorsePhrase morsePhrase;
	};


	class MorseCodeTreeNode final : protected BinarySearchTreeNode<MorsePhraseCharPair> {
	private:
		MorseCodeTreeNode* insert(MorsePhraseCharPair& d) override {return nullptr;} //= delete;
		/*void printSubtree(const short spacingIndex, const MorseCodeTreeNode* node, bool lesser) {
			if(node) {
				for(short i = 0; i < spacingIndex; i += 1) {
					Serial.print(' ');
				}

				Serial.print(lesser ? "├── " : "└── ");
				Serial.println(node->data->character);

				printSubtree(spacingIndex + 4, node->lesserNode, true);
				printSubtree(spacingIndex + 4, node->greaterNode, false);
			}
		}*/

	public:
		MorseCodeTreeNode(MorsePhraseCharPair& newData, MorseCodeTreeNode* newParent) {
			data = &newData;
			parentNode = newParent;
			lesserNode = nullptr;
			greaterNode = nullptr;
		}

		MorseCodeTreeNode* addNode(MorsePhraseCharPair& dataToInsert, unsigned short depth = 0) {
			if (dataToInsert == *data) {
				return nullptr; //No duplicates allowed!
			}

			if(dataToInsert.isLessThan(depth)) {
				if(lesserNode == nullptr) {
					lesserNode = new MorseCodeTreeNode(dataToInsert, this);
					return (MorseCodeTreeNode*) lesserNode;
				} else {
					return ((MorseCodeTreeNode*) lesserNode)->addNode(dataToInsert, depth + 1);
				}
			} else {
				if(greaterNode == nullptr) {
					greaterNode = new MorseCodeTreeNode(dataToInsert, this);
					return (MorseCodeTreeNode*) greaterNode;
				} else {
					return ((MorseCodeTreeNode*) greaterNode)->addNode(dataToInsert, depth + 1);
				}
			}
		}

		MorsePhraseCharPair* lookup(MorsePhrase& phraseToConvert) {
			if(phraseToConvert.readAt(0) == NOTHING) {
				return nullptr;
			}

			if(phraseToConvert == data->morsePhrase) {
				return data;
			}

			if(phraseToConvert < data->morsePhrase) {
				if(!lesserNode) {
					return nullptr;
				}

				return ((MorseCodeTreeNode*) lesserNode)->lookup(phraseToConvert);
			} else {
				if(!greaterNode) {
					return nullptr;
				}

				return ((MorseCodeTreeNode*) greaterNode)->lookup(phraseToConvert);
			}
		}

		//void print() {printSubtree(0, this, false);}
	};
}

#endif
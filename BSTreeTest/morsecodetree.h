#ifndef MORSECODETREE_H
#define MORSECODETREE_H

#include "binarysearchtree.h"

/*

    └── 
│       ├── e				(less than root)
│           ├── i			(less than e)

        └── a				(greater than root)
            └── n			(greater than a)


*/

/*
   └── 
│       ├── e
│           ├── i
       └── t
           └── a			(wrong)
*/


struct ArrObj {
	ArrObj(const char v = '\0') {value = v;}

	bool operator==(const ArrObj& o) {return value == o.value;}
	bool operator!=(const ArrObj& o) {return value != o.value;}
	bool operator>(const ArrObj& o) {
		if(value == o.value) return false;
		return (value == '-');
	};
	bool operator<(const ArrObj& o) {
		if(value == o.value) return false;
		return (value == '.');
	};

	char value;
};


struct CustObj {
	CustObj() {val = new ArrObj[VAL_ARR_LEN];}

	ArrObj& lastVal() {return val[firstOpenIndex - 1];}

	void push(char o) {
		if(firstOpenIndex == VAL_ARR_LEN) {
			return;
		}

		val[firstOpenIndex++] = *new ArrObj(o);
	}

	void printString() {
		for(int i = 0; i < VAL_ARR_LEN; i += 1) {
			if(val[i].value == '\0') {
				break;
			}

			Serial.print(val[i].value);
		}
	}

	ArrObj *val;
	const unsigned short VAL_ARR_LEN = 6;
	unsigned short firstOpenIndex = 0;
};


struct CustPair {
	CustPair(CustObj* k, char v) {key = k; value = v;}
	CustObj* key;
	char value;

	bool isLessThan(unsigned short depth) {
		return key->val[depth] == '.' ? true : false;
	}

	bool operator==(CustPair& o) {
		return value == o.value;
	}

	void printString() {
		key->printString();
		Serial.print(" : ");
		Serial.println(value);
	}
};


class MorseCodeTreeNode : public BinarySearchTreeNode<CustPair> {
public:
	MorseCodeTreeNode(CustPair*, MorseCodeTreeNode*);

	MorseCodeTreeNode* insert(CustPair*, unsigned short = 0);

	void print();
protected: // Might be making copies...
	MorseCodeTreeNode* parentNode;
	MorseCodeTreeNode* lesserNode;
	MorseCodeTreeNode* greaterNode;
private:
	void printSubtree(const short, const MorseCodeTreeNode*, bool);
};


MorseCodeTreeNode::MorseCodeTreeNode(CustPair* newData, MorseCodeTreeNode* newParent) {
	data = newData;
	parentNode = newParent;
	lesserNode = nullptr;
	greaterNode = nullptr;
}


MorseCodeTreeNode* MorseCodeTreeNode::insert(CustPair* dataToInsert, unsigned short depth) {
	if(dataToInsert == nullptr) {
		return nullptr;
	}

	if (*dataToInsert == *data) {
		return nullptr; //No duplicates allowed!
	}

	if(dataToInsert->isLessThan(depth)) { //*dataToInsert < *data) {
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


void MorseCodeTreeNode::print() {
	printSubtree(0, this, false);
}


void MorseCodeTreeNode::printSubtree(const short spacingIndex, const MorseCodeTreeNode* node, bool lesser) {
	if(node) {
		//if(node->data) {
			for(int i = 0; i < spacingIndex; i += 1) {
				Serial.print(' ');
			}

			Serial.print(lesser ? "├── " : "└── ");
			if(node->data->value != '\0') {
				Serial.println(node->data->value);
			} else {
				Serial.println("[]");
			}

			//if(node->parentNode) Serial.println(node->parentNode->data->value);
		//}

		printSubtree(spacingIndex + 4, node->lesserNode, true);
		printSubtree(spacingIndex + 4, node->greaterNode, false);
	}
}

#endif
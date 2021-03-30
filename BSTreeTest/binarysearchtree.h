#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H

#include "HardwareSerial.h"


template <class T>
class BinarySearchTreeNode {
public:
	BinarySearchTreeNode(T* d, BinarySearchTreeNode<T>* p);
	~BinarySearchTreeNode(); //WARNING: destructor will attempt to delete all linked children nodes!

	BinarySearchTreeNode<T>* getParentNode() {return parentNode;}
	BinarySearchTreeNode<T>* getLesserNode() {return lesserNode;}
	BinarySearchTreeNode<T>* getGreaterNode() {return greaterNode;}

	BinarySearchTreeNode<T>* insert(T* d);
	BinarySearchTreeNode<T>* remove(); //NOTE: Never remove from Morse Code tree

	short getBalance() const {return greaterNode->getDepth() - lesserNode->getDepth();}
	void balanceSubtree();

	void print();
private:
	BinarySearchTreeNode<T>* parentNode;
	BinarySearchTreeNode<T>* lesserNode;
	BinarySearchTreeNode<T>* greaterNode;

	static constexpr unsigned short CORRECTABLE_BALANCE_VALUE = 2;

	BinarySearchTreeNode<T>* getSmallest();

	short getDepth() const;

	void printSubtree(const short, const BinarySearchTreeNode<T>*, bool);

	T* data;
};


template <class T>
BinarySearchTreeNode<T>::BinarySearchTreeNode(T* d, BinarySearchTreeNode<T>* p) {
	data = d;
	parentNode = p;
	lesserNode = nullptr;
	greaterNode = nullptr;
}


template <class T>
BinarySearchTreeNode<T>::~BinarySearchTreeNode() {
	//delete parentNode; //cannot delete parent and children, otherwise two-way pointers will cause loop
	delete lesserNode;
	delete greaterNode;
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::insert(T* d) {
	if(d == nullptr) {
		return nullptr;
	}

	if (*d == *data) {
		return nullptr; //No duplicates allowed!
	}

	if(*d < *data) {
		if(lesserNode == nullptr) {
			lesserNode = new BinarySearchTreeNode(d, this);
			return lesserNode;
		} else {
			return lesserNode->insert(d);
		}
	} else {
		if(greaterNode == nullptr) {
			greaterNode = new BinarySearchTreeNode(d, this);
			return greaterNode;
		} else {
			return greaterNode->insert(d);
		}
	}
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::remove() {
	if(parentNode == nullptr) {
		return nullptr; //shouldn't delete root node this way (maybe possible if we scrap the parent class BinarySearchTree, which keeps track of root node)
	}

	if(!lesserNode && !greaterNode) { //no children
		if(parentNode->lesserNode == this) {
			parentNode->lesserNode = nullptr;
		} else {
			parentNode->greaterNode = nullptr;
		}
		return this;
	}

	if(lesserNode && !greaterNode) { //only one child (lesser)
		if(parentNode->lesserNode == this) {
			parentNode->lesserNode = lesserNode;
		} else {
			parentNode->greaterNode = lesserNode;
		}
		return this;
	}

	if(!lesserNode && greaterNode) { //only one child (greater)
		if(parentNode->lesserNode == this) {
			parentNode->lesserNode = greaterNode;
		} else {
			parentNode->greaterNode = greaterNode;
		}
		return this;
	}

	if(lesserNode && greaterNode) { //two children
		//find smallest node from all nodes on greaterNode side
		BinarySearchTreeNode<T>* smallestChild = greaterNode->getSmallest();
		//replace this node with found node
			//create copy of found node
			
			//assign lesser and greater nodes from this node to new copy of smallest
			//assign nullptr to this node's children
			//find whether this node is greater or lesser in parent
			//assign new copy of smallest as parent's child, replacing this node
		//remove original smallest node
		//return this now-removed node
	}
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::getSmallest() {
	if(lesserNode == nullptr) {
		return this;
	}

	return lesserNode->getSmallest();
}


template <class T>
void BinarySearchTreeNode<T>::print() {
	printSubtree(0, this, false);
}


template <class T>
void BinarySearchTreeNode<T>::printSubtree(const short spacingIndex, const BinarySearchTreeNode<T>* node, bool lesser) {
	if(node) {
		if(node->data) {
			if(lesser) {
				Serial.print("│   ");
			} else {
				Serial.print("    ");
			}

			for(int i = 0; i < spacingIndex; i += 1) {
				Serial.print(' ');
			}
			Serial.print(lesser ? "├── " : "└── ");
			Serial.println(node->data->value);
		}

		printSubtree(spacingIndex + 4, node->lesserNode, true);
		printSubtree(spacingIndex + 4, node->greaterNode, false);
	}
}


#endif
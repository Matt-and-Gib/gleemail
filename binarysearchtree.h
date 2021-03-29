#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H

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
private:
	BinarySearchTreeNode<T>* parentNode;
	BinarySearchTreeNode<T>* lesserNode;
	BinarySearchTreeNode<T>* greaterNode;

	static constexpr unsigned short CORRECTABLE_BALANCE_VALUE = 2;

	BinarySearchTreeNode<T>* findSmallest(T* d);

	short getDepth() const;

	T* data;
};


template <class T>
BinarySearchTreeNode<T>::BinarySearchTreeNode(T* d, BinarySearchTreeNode<T>* p) {
	data = d;
	parentNode = p;
	lesserNode = nullptr;
	greaterNode = nullptr;
	//height = 0;
}


template <class T>
BinarySearchTreeNode<T>::~BinarySearchTreeNode() {
	//delete parentNode; //cannot delete parent and children, otherwise two-way pointers will cause loop
	delete lesserNode;
	delete greaterNode;
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::insert(T* d) {
	if (d == data) {
		return nullptr; //No duplicates allowed!
	}

	if(d < data) {
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
		BinarySearchTreeNode<T>* smallestChild = greaterNode->findSmallest(this->data);
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
void BinarySearchTreeNode<T>::correctSubtree() {
	/*
		Parent lesserNode than grandparent:
			Two cases:
				1. Child is less than parent
				2. Child is greater than parent

			Case 1:
				1. Grandparent becomes greaterNode of Parent
				2. Parent becomes root

			Case 2:
				1. Parent becomes lesserNode of Child
				2. Child becomes lesserNode of Grandparent
				3. Go to case 1

		Parent greaterNode of Grandparent:
			Two cases:
				1. Child is greater than parent
				2. Child is less than parent

			Case 1:
				1. Grandparent becomes lesserNode of Parent
				2. Parent becomes root

			Case 2:
				1. Parent becomes greaterNode of Child
				2. Child becomes greaterNode of Grandparent
				3. Go to case 1
	*/
}


template <class T>
void BinarySearchTreeNode<T>::balanceSubtree() {
	//REMEMBER: balancing a binary search tree is not the same as sorting it!
	//difference between leaf nodes height must not be greater than one

	//BALANCE: (max height of right subtree) - (max height of left subtree)
	//HEIGHT: number of children


	const short currentBalance = getBalance(); //make static

	if(abs(currentBalance) < CORRECTABLE_BALANCE_VALUE && getDepth() < CORRECTABLE_BALANCE_VALUE) { //Subtree is balanced
		return;
		 X
		/ \
		x x
	}

	if(abs(currentBalance) == CORRECTABLE_BALANCE_VALUE && getDepth() == CORRECTABLE_BALANCE_VALUE) { //Correctable subtree
		correctSubtree();
		return;
	}

	if(currentBalance < 0) {
		lesserNode->balanceSubtree(); //we need to
		correctSubtree();
	} else {
		greaterNode->balanceSubtree(); //go deeper
		correctSubtree();
	}
}


template <class T>
short BinarySearchTreeNode<T>::getDepth() const {
	if(!lesserNode && !greaterNode) {
		return 0;
	}

	if(lesserNode && !greaterNode) {
		return lesserNode->getDepth() + 1;
	}

	if(!lesserNode && greaterNode) {
		return greaterNode->getDepth() + 1;
	}

	return max(lesserNode->getDepth(), greaterNode->getDepth()) + 1;
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::findSmallest(T* d) {
	if(d == nullptr) {
		return nullptr;
	}

	if(data == d) {
		return this;
	}

	return lesserNode->findSmallest(d);
}


/*template <class T>
class BinarySearchTree {
public:
	BinarySearchTree();
	~BinarySearchTree();

	BinarySearchTreeNode<T>* getRootNode() {return rootNode;}
	void setRootNode(BinarySearchTreeNode<T>* newRoot) {rootNode = newRoot;}

	BinarySearchTreeNode<T>* insert(T*);
	BinarySearchTreeNode<T>* find(T*);

	void balance();
private:
	BinarySearchTreeNode<T>* rootNode;
};

template <class T>
BinarySearchTree<T>::BinarySearchTree() {
	rootNode = nullptr;
}


template <class T>
BinarySearchTree<T>::~BinarySearchTree() {
	delete rootNode;
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTree<T>::insert(T* newData) {
	if(rootNode == nullptr) {
		rootNode = new BinarySearchTreeNode<T>(newData, nullptr);
		return rootNode;
	}

	return rootNode->insert(newData);
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTree<T>::find(T* data) {
	unsigned short currentDepth = 0;
	//update currentDepth as we descend, use value to check only one index of each node (phraseArray[currentDepth] == data[currentDepth])
}


template <class T>
void BinarySearchTree<T>::balance() {
	while(abs(rootNode->getBalance()) > 1) {
		rootNode->balanceSubtree();
	}
}*/

#endif
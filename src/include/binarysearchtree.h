#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H


template <class T>
class BinarySearchTreeNode {
private:
	T* const data;
	BinarySearchTreeNode* lesserChild = nullptr;
	BinarySearchTreeNode* greaterChild = nullptr;
public:
	explicit BinarySearchTreeNode(const T& d) : data{&d} {}
	virtual ~BinarySearchTreeNode();

	T* getData() {return data;}
	BinarySearchTreeNode* getLesserChild() {return lesserChild;}
	void setLesserChild(const BinarySearchTreeNode& l) {lesserChild = &l;}
	BinarySearchTreeNode* getGreaterChild() {return greaterChild;}
	void setGreaterChild(const BinarySearchTreeNode& g) {greaterChild = &g;}

	virtual BinarySearchTreeNode* addNode(BinarySearchTreeNode*);
};


template <class T>
inline BinarySearchTreeNode<T>::~BinarySearchTreeNode() {
	delete greaterChild;
	greaterChild = nullptr;

	delete lesserChild;
	lesserChild = nullptr;

	delete data;
}


template <class T>
inline BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::addNode(BinarySearchTreeNode* n) {
	if(!n) {
		return nullptr;
	}

	BinarySearchTreeNode* currentNode = this;
	while(currentNode != nullptr) {
		if(n == currentNode || *(n->getData()) == *(currentNode->getData())) {
			return nullptr;
		}

		if(*(n->getData()) < *(currentNode->getData())) {
			if(currentNode->getLesserChild()) {
				currentNode = currentNode->getLesserChild();
			} else {
				currentNode->setLesserChild(n);
				return n;
			}
		} else {
			if(currentNode->getGreaterChild()) {
				currentNode = currentNode->getGreaterChild();
			} else {
				currentNode->setGreaterChild(n);
				return n;
			}
		}
	}

	return nullptr;
}

#endif
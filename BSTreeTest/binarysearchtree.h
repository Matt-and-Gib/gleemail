#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H


template <class T>
class BinarySearchTreeNode {
public:
	BinarySearchTreeNode(T* d, BinarySearchTreeNode<T>* p);
	
	BinarySearchTreeNode<T>* getParentNode() {return parentNode;}
	BinarySearchTreeNode<T>* getLesserNode() {return lesserNode;}
	BinarySearchTreeNode<T>* getGreaterNode() {return greaterNode;}

	void insert(T* d);
	BinarySearchTreeNode<T>* remove();
	BinarySearchTreeNode<T>* remove(BinarySearchTreeNode<T>*);
private:
	BinarySearchTreeNode<T>* parentNode;
	BinarySearchTreeNode<T>* lesserNode;
	BinarySearchTreeNode<T>* greaterNode;

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
void BinarySearchTreeNode<T>::insert(T* d) {
	if (d == data) {
		return; //No duplicates allowed!
	}

	if(d < data) {
		if(lesserNode == nullptr) {
			lesserNode = new BinarySearchTreeNode(d, this);
		} else {
			lesserNode.insert(d);
		}
	} else {
		if(greaterNode == nullptr) {
			greaterNode = new BinarySearchTreeNode(d, this);
		} else {
			greaterNode.insert(d);
		}
	}

	//balance tree
}

/* //Maybe process delete from BST class?
template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode::remove() {
	if(lesserNode == nullptr && greaterNode == nullptr) {
		parentNode->remove(this);
	}
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode::remove(BinarySearchTreeNode<T>* child) {

}
*/


template <class T>
class BinarySearchTree {
public:
	BinarySearchTree();

	BinarySearchTreeNode<T>* getRootNode() {return rootNode;}
	void setRootNode(BinarySearchTreeNode<T>* newRoot) {rootNode = newRoot;}
private:
	BinarySearchTreeNode<T>* rootNode;
};

template <class T>
BinarySearchTree<T>::BinarySearchTree() {
	rootNode = nullptr;
}

#endif
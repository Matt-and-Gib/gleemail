#ifndef BINARYSEARCHTREE_H
#define BINARYSEARCHTREE_H

template <class T>
class BinarySearchTreeNode {
public:
	BinarySearchTreeNode();
	BinarySearchTreeNode(T* d, BinarySearchTreeNode<T>* p);
	~BinarySearchTreeNode(); //WARNING: destructor will attempt to delete all linked children nodes!

	BinarySearchTreeNode<T>* getParentNode() {return parentNode;}
	BinarySearchTreeNode<T>* getLesserNode() {return lesserNode;}
	BinarySearchTreeNode<T>* getGreaterNode() {return greaterNode;}

	BinarySearchTreeNode<T>* insert(T& d);
	BinarySearchTreeNode<T>* remove(); //NOTE: Never remove from Morse Code tree

	T* getData() {return data;}
protected:
	BinarySearchTreeNode<T>* parentNode;
	BinarySearchTreeNode<T>* lesserNode;
	BinarySearchTreeNode<T>* greaterNode;

	BinarySearchTreeNode<T>* getSmallest();

	T* data;
};


template <class T>
BinarySearchTreeNode<T>::BinarySearchTreeNode() {
	data = nullptr;
	parentNode = nullptr;
	lesserNode = nullptr;
	greaterNode = nullptr;
}


template <class T>
BinarySearchTreeNode<T>::BinarySearchTreeNode(T* d, BinarySearchTreeNode<T>* p) {
	data = d;
	parentNode = p;
	lesserNode = nullptr;
	greaterNode = nullptr;
}


template <class T>
BinarySearchTreeNode<T>::~BinarySearchTreeNode() {
	delete lesserNode;
	delete greaterNode;
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::insert(T& d) {
	if (d == *data) {
		return nullptr; //No duplicates allowed!
	}

	if(d < *data) {
		if(lesserNode == nullptr) {
			lesserNode = new BinarySearchTreeNode(&d, this);
			return lesserNode;
		} else {
			return lesserNode->insert(d);
		}
	} else {
		if(greaterNode == nullptr) {
			greaterNode = new BinarySearchTreeNode(&d, this);
			return greaterNode;
		} else {
			return greaterNode->insert(d);
		}
	}
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::remove() {
	if(parentNode == nullptr) {
		return nullptr; //shouldn't delete root node this way. delete root instead
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

#endif
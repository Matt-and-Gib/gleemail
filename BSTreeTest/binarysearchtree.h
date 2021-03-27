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
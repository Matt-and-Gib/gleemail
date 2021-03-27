#ifndef BSTREE_H
#define BSTREE_H


template <class T>
class BinarySearchTreeNode {
public:
	BinarySearchTreeNode<T>();
	BinarySearchTreeNode<T>(T* key);

	static BinarySearchTreeNode<T>* balance(BinarySearchTreeNode<T>*);
	static BinarySearchTreeNode<T>* insert(BinarySearchTreeNode<T>*, T*);

	T* getKey() {return key;}

	void setParentNode(BinarySearchTreeNode<T>* node) {parentNode = node;}
	BinarySearchTreeNode<T>* getSmallerNode() {return smallerNode;}
	BinarySearchTreeNode<T>* getLargerNode() {return largerNode;}
private:
	T* key;
	BinarySearchTreeNode<T>* parentNode;
	BinarySearchTreeNode<T>* smallerNode;
	BinarySearchTreeNode<T>* largerNode;
};


template <class T>
class BinarySearchTree {
public:
	BinarySearchTree<T>();
	BinarySearchTree<T>(BinarySearchTreeNode<T>*);
	~BinarySearchTree<T>();

	BinarySearchTreeNode<T>* insert(BinarySearchTreeNode<T>*);
private:
	BinarySearchTreeNode<T>* rootNode;
};


#endif

template <class T>
BinarySearchTreeNode<T>::BinarySearchTreeNode() {
	key = nullptr;
	parentNode = nullptr;
	smallerNode = nullptr;
	largerNode = nullptr;
}


template <class T>
BinarySearchTreeNode<T>::BinarySearchTreeNode(T* keyValue) {
	key = keyValue;
	parentNode = nullptr;
	smallerNode = nullptr;
	largerNode = nullptr;
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::balance(BinarySearchTreeNode<T>* localRoot) {
	//TODO: balance logic
}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTreeNode<T>::insert(BinarySearchTreeNode<T>* localRoot, T* key) {
	if(localRoot == nullptr) {
		localRoot = new BinarySearchTreeNode<T>(key);
		return localRoot;
	}

	if(key == localRoot->getKey()) {
		return nullptr;
	}

	if(key < localRoot->getKey()) {
		localRoot->getSmallerNode()->insert(localRoot->getSmallerNode(), key);
	} else {
		localRoot->getLargerNodee()->insert(localRoot->getLargerNode(), key);
	}

	localRoot = localRoot->balance();
}


template <class T>
BinarySearchTree<T>::BinarySearchTree() {
	rootNode = nullptr;
}


template <class T>
BinarySearchTree<T>::BinarySearchTree(BinarySearchTreeNode<T>* root) {
	rootNode = root;
}


template <class T>
BinarySearchTree<T>::~BinarySearchTree() {

}


template <class T>
BinarySearchTreeNode<T>* BinarySearchTree<T>::insert(BinarySearchTreeNode<T>* node) {
	return rootNode->insert(node);
}
#include "bstree.h"


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
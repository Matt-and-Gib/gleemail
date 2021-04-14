#ifndef LINKEDLIST_H
#define LINKEDLIST_H

template <class T>
struct LinkedListNode {
	LinkedListNode(T*);
	~LinkedListNode();

	LinkedListNode<T>* node;
	T* data;

	void enqueue(T*);
	LinkedListNode<T>* getNode() {return node;}
	T* getData() {return data;}
};


template <class T>
LinkedListNode<T>::LinkedListNode(T* d) {
	data = d;
	node = nullptr;
}


template <class T>
LinkedListNode<T>::~LinkedListNode() {
	delete &data;
	delete[] node;
}


template <class T>
void LinkedListNode<T>::enqueue(T* o) {
	if(!node) {
		node = new LinkedListNode<T>(o);
	} else {
		node->enqueue(o);
	}
}


template <class T>
class LinkedList {
private:
	LinkedListNode<T>* root;
public:
	LinkedList() {
		root = nullptr;
	}

	void enqueue(T*); //puts node at end of queue
	LinkedListNode<T>* dequeue(); //take first node
	LinkedListNode<T>* peek(); //returns first node
};


template <class T>
void LinkedList<T>::enqueue(T* o) {
	if(!root) {
		root = new LinkedListNode<T>(o);
	} else {
		root->enqueue(o);
	}
}


template <class T>
LinkedListNode<T>* LinkedList<T>::dequeue() {
	if(!root) {
		return nullptr;
	}

	LinkedListNode<T>* oldRoot = root;

	if(!root->getNode()) {
		root = nullptr;
		return oldRoot;
	}

	root = root->getNode();
	oldRoot->node = nullptr;
	return oldRoot;
}


template <class T>
LinkedListNode<T>* LinkedList<T>::peek() {
	if(!root) {
		return nullptr;
	}

	return root;
}

#endif
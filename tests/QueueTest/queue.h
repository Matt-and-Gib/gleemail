#ifndef QUEUE_H
#define QUEUE_H

template <class T>
struct QueueNode {
	QueueNode(T*);
	~QueueNode();

	QueueNode<T>* node;
	T* data;

	void enqueue(T*);
	QueueNode<T>* getNode() {return node;}
	T* getData() {return data;}
};


template <class T>
QueueNode<T>::QueueNode(T* d) {
	data = d;
	node = nullptr;
}


template <class T>
QueueNode<T>::~QueueNode() {
	delete &data;
	delete[] node;
}


template <class T>
void QueueNode<T>::enqueue(T* o) {
	if(!node) {
		node = new QueueNode<T>(o);
	} else {
		node->enqueue(o);
	}
}


template <class T>
class Queue {
private:
	QueueNode<T>* root;
public:
	Queue() {
		root = nullptr;
	}

	void enqueue(T*); //puts node at end of queue
	QueueNode<T>* dequeue(); //take first node
	QueueNode<T>* peek(); //returns first node

	//QueueNode<T>* remove(T&, QueueNode<T>*);
	QueueNode<T>* remove(T&);
};


template <class T>
void Queue<T>::enqueue(T* o) {
	if(!root) {
		root = new QueueNode<T>(o);
	} else {
		root->enqueue(o);
	}
}


template <class T>
QueueNode<T>* Queue<T>::dequeue() {
	if(!root) {
		return nullptr;
	}

	QueueNode<T>* oldRoot = root;

	if(!root->getNode()) {
		root = nullptr;
		return oldRoot;
	}

	root = root->getNode();
	oldRoot->node = nullptr;
	return oldRoot;
}


template <class T>
QueueNode<T>* Queue<T>::peek() {
	if(!root) {
		return nullptr;
	}

	return root;
}


/*template <class T>
QueueNode<T>* Queue<T>::remove(T& match, QueueNode<T>* parent = nullptr) {
	if(*this->data == match) {
		if(!parent) {
			return dequeue();
		}

		QueueNode<T>* matchedNode = this;
		parent->node = this->node;
		this->node = nullptr;
		return matchedNode;
	}

	return this->node->remove(match, this->node);
}*/


template <class T>
QueueNode<T>* Queue<T>::remove(T& match) {
	QueueNode<T>* parentNode = nullptr;
	QueueNode<T>* currentNode = root;
	while(currentNode) {
		if(*(currentNode->data) == match) {
			break;
		}

		parentNode = currentNode;
		currentNode = currentNode->node;
	}

	if(currentNode) {
		if(!parentNode) {
			return dequeue();
		}

		parentNode->node = currentNode->node;
		currentNode->node = nullptr;
	}

	return currentNode;
}

#endif
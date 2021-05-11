#ifndef QUEUE_H
#define QUEUE_H


template <class T>
class QueueNode {
private:
	QueueNode<T>* node;
	T* data;
public:
	QueueNode(T* d) {
		data = d;
		node = nullptr;
	}

	~QueueNode() {
		delete &data;
		delete[] node;
	}

	void setNode(QueueNode<T>* n) {node = n;} //possible memory leak
	QueueNode<T>* getNode() {return node;}
	void setData(T* d) {data = d;} //possible memory leak
	T* getData() {return data;}

	QueueNode<T>* enqueue(T* o) {
		if(!node) {
			node = new QueueNode<T>(o);
			return node;
		} else {
			return node->enqueue(o);
		}
	}
};


template <class T>
class Queue {
private:
	QueueNode<T>* root;
public:
	Queue() {
		root = nullptr;
	}

	QueueNode<T>* enqueue(T*); //puts node at end of queue
	QueueNode<T>* dequeue(); //take first node
	QueueNode<T>* peek(); //returns first node

	QueueNode<T>* find(T&);

	//QueueNode<T>* remove(T&, QueueNode<T>*);
	QueueNode<T>* remove(T&);
	QueueNode<T>* remove(QueueNode<T>&);
};


template <class T>
QueueNode<T>* Queue<T>::enqueue(T* o) {
	if(!root) {
		root = new QueueNode<T>(o);
		return root;
	} else {
		return root->enqueue(o);
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
	oldRoot->setNode(nullptr);
	return oldRoot;
}


template <class T>
QueueNode<T>* Queue<T>::peek() {
	if(!root) {
		return nullptr;
	}

	return root;
}


template <class T>
QueueNode<T>* Queue<T>::find(T& f) {
	QueueNode<T>* currentNode = root;
	while(currentNode != nullptr) {
		if(*currentNode->getData() == f) {
			return currentNode;
		}

		currentNode = currentNode->getNode();
	}

	return nullptr;
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
		if(*(currentNode->getData()) == match) {
			break;
		}

		parentNode = currentNode;
		currentNode = currentNode->getNode();
	}

	if(currentNode) {
		if(!parentNode) {
			return dequeue();
		}

		parentNode->setNode(currentNode->getNode());
		currentNode->setNode(nullptr);
	}

	return currentNode;
}


template <class T>
QueueNode<T>* Queue<T>::remove(QueueNode<T>& match) {
	//return remove(*match.getData());
	
	QueueNode<T>* parentNode = nullptr;
	QueueNode<T>* currentNode = root;
	while(currentNode) {
		if(currentNode == &match) {
			break;
		}

		parentNode = currentNode;
		currentNode = currentNode->getNode();
	}

	if(currentNode) {
		if(!parentNode) {
			return dequeue();
		}

		parentNode->setNode(currentNode->getNode());
		currentNode->setNode(nullptr);
	}

	return currentNode;
}

#endif
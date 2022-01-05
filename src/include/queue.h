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
		delete data;
		delete node; //Note: This will cascade-delete all children if called on a node with children. Always use in tandem with remove() or dequeue()
	}

	void setNode(QueueNode<T>* n) {node = n;} //possible memory leak
	QueueNode<T>* getNode() {return node;}
	void setData(T* d) {data = d;} //possible memory leak
	T* getData() {return data;}

	QueueNode<T>* enqueue(T* o) {
		if(node == nullptr) {
			node = new QueueNode<T>(o);
			return node;
		} else {
			return node->enqueue(o);
		}
	}

	unsigned short countChildren() const {return !node ? 0 : node->countChildren() + 1;}
};


template <class T>
class Queue {
private:
	QueueNode<T>* root;
public:
	Queue() {
		root = nullptr;
	}
	~Queue() {
		delete root; //Note: All children will be recursively deleted. This could be expected behavior, or it could do bad things. Just, be aware.
	}

	QueueNode<T>* enqueue(T*);
	QueueNode<T>* dequeue();
	QueueNode<T>* peek();

	QueueNode<T>* find(T&);

	//QueueNode<T>* remove(T&, QueueNode<T>*);
	QueueNode<T>* remove(T&);
	QueueNode<T>* remove(QueueNode<T>&);

	bool empty() const {return root == nullptr;}

	unsigned short length() const {return !root ? 0 : root->countChildren() + 1;}
};


template <class T>
inline QueueNode<T>* Queue<T>::enqueue(T* o) {
	if(root == nullptr) {
		root = new QueueNode<T>(o);
		return root;
	} else {
		return root->enqueue(o);
	}
}


template <class T>
inline QueueNode<T>* Queue<T>::dequeue() {
	if(root == nullptr) {
		return nullptr;
	}

	QueueNode<T>* oldRoot = root;

	if(root->getNode() == nullptr) {
		root = nullptr;
		return oldRoot;
	}

	root = root->getNode();
	oldRoot->setNode(nullptr);
	return oldRoot;
}


template <class T>
inline QueueNode<T>* Queue<T>::peek() {
	return root;
}


template <class T>
inline QueueNode<T>* Queue<T>::find(T& f) {
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
inline QueueNode<T>* Queue<T>::remove(T& match, QueueNode<T>* parent = nullptr) {
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
inline QueueNode<T>* Queue<T>::remove(T& match) {
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
inline QueueNode<T>* Queue<T>::remove(QueueNode<T>& match) {
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
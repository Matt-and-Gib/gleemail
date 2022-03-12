#ifndef QUEUE_H
#define QUEUE_H


template <class T>
class QueueNode final {
private:
	T* data;
	QueueNode<T>* node;

public:
	explicit QueueNode(T* d) : data{d}, node{nullptr} {}
	QueueNode(const QueueNode&) = delete;
	QueueNode(QueueNode&&) = delete;
	QueueNode& operator=(const QueueNode&) = delete;
	QueueNode& operator=(QueueNode&&) = delete;
	~QueueNode() {
		delete data;
		delete node; //Note: This will cascade-delete all children if called on a node with children. Always use in tandem with remove() or dequeue()
	}

	void setNode(QueueNode<T>* n) {node = n;} //possible memory leak
	[[nodiscard]] QueueNode<T>* getNode() {return node;}
	void setData(T* d) {data = d;} //possible memory leak
	[[nodiscard]] T* getData() {return data;}
	[[nodiscard]] T* readData() const {return data;}

	QueueNode<T>* enqueue(T* o) {
		if(node == nullptr) {
			node = new QueueNode<T>(o);
			return node;
		} else {
			return node->enqueue(o);
		}
	}

	[[nodiscard]] unsigned short countChildren() const {return !node ? 0 : node->countChildren() + 1;}
};


template <class T>
class Queue final {
private:
	QueueNode<T>* root = nullptr;

public:
	Queue() : root{nullptr} {}
	Queue(const Queue&) = delete;
	Queue(Queue&&) = delete;
	Queue& operator=(const Queue&) = delete;
	Queue& operator=(Queue&&) = delete;
	~Queue() {
		delete root; //Note: All children will be recursively deleted. This could be expected behavior, or it could do bad things. Just, be aware.
	}

	QueueNode<T>* enqueue(T*);
	[[nodiscard]] QueueNode<T>* dequeue();
	[[nodiscard]] QueueNode<T>* peek();

	QueueNode<T>* find(const T*);

	//QueueNode<T>* remove(T&, QueueNode<T>*);
	[[nodiscard]] QueueNode<T>* remove(const T&);
	QueueNode<T>* remove(const QueueNode<T>&);

	[[nodiscard]] bool empty() const {return root == nullptr;}
	[[nodiscard]] unsigned short length() const {return !root ? 0 : root->countChildren() + 1;}
};


template <class T>
inline QueueNode<T>* Queue<T>::enqueue(T* o) {
	if(o == nullptr) {
		return nullptr;
	}

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
	root = oldRoot->getNode();
	oldRoot->setNode(nullptr);
	return oldRoot;
}


template <class T>
inline QueueNode<T>* Queue<T>::peek() {
	return root;
}


template <class T>
inline QueueNode<T>* Queue<T>::find(const T* f) {
	QueueNode<T>* currentNode = root;
	while(currentNode != nullptr) {
		if(*(currentNode->getData()) == *f) {
			break;
		}

		currentNode = currentNode->getNode();
	}

	return currentNode;
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
inline QueueNode<T>* Queue<T>::remove(const T& match) {
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
inline QueueNode<T>* Queue<T>::remove(const QueueNode<T>& match) {
	return remove(*match.readData());
	
	/*QueueNode<T>* parentNode = nullptr;
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

	return currentNode;*/
}

#endif
#ifndef QUEUE_H
#define QUEUE_H

//#include "HardwareSerial.h"


template <class T>
class QueueNode {
private:
	QueueNode<T>* node;
	T* data;
public:
	QueueNode(T* d) {
		Serial.println(F("inside queuenode constructor"));
		data = d;
		node = nullptr;
		Serial.println(F("queuenode constructor done"));
	}

	~QueueNode() {
		//Serial.println("QueueNode destructor: Deleting data...");
		delete data;
		//Serial.println("QueueNode destructor: Deleting node...");
		delete node;
		//Serial.println("QueueNode destructor: Done!");
	}

	void setNode(QueueNode<T>* n) {node = n;} //possible memory leak
	QueueNode<T>* getNode() {return node;}
	void setData(T* d) {data = d;} //possible memory leak
	T* getData() {return data;}

	QueueNode<T>* enqueue(T* o) {
		Serial.println(F("inside queuenode enqueue"));

		if(!node) {
			Serial.println(F("queuenode enqueue: node is nullptr"));
			node = new QueueNode<T>(o);
			return node;
		} else {
			Serial.println(F("inside queuenode: doesn't have child node"));
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
	~Queue() {
		delete root;
	}

	QueueNode<T>* enqueue(T*); //puts node at end of queue
	QueueNode<T>* dequeue(); //take first node
	QueueNode<T>* peek(); //returns first node

	QueueNode<T>* find(T&);

	//QueueNode<T>* remove(T&, QueueNode<T>*);
	QueueNode<T>* remove(T&);
	QueueNode<T>* remove(QueueNode<T>&);

	//bool empty() {return root == nullptr;}
};


template <class T>
QueueNode<T>* Queue<T>::enqueue(T* o) {
	Serial.println(F("inside enqueue"));

	Serial.println(root == nullptr ? "root == nullptr" : "root != nullptr");

	if(!root) {
		Serial.println(F("root == nullptr"));
		root = new QueueNode<T>(o);
		Serial.println(F("created new node"));
		return root;
	} else {
		Serial.println(F("root exists, enqueue into child"));
		return root->enqueue(o);
	}
}


template <class T>
QueueNode<T>* Queue<T>::dequeue() {
	if(!root) {
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
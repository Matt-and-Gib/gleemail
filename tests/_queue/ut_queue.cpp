#include "queue.h"
#include <iostream>
#include <stdlib.h>


class TestingObject {
private:
	int data = -1;
public:
	TestingObject(const int d) : data{d} {}
	int getData() {return data;}
};


int test_queue_no_children_count() {
	Queue<TestingObject> testQ;

	if((testQ.empty() == true) && (testQ.length() == 0) && (testQ.peek() == nullptr)) {
		return 0;
	} else {
		return 1;
	}
}


int test_queue_add_node() {
	const unsigned short NQDOBJ_VALUE = 69;

	Queue<TestingObject> testQ;

	QueueNode<TestingObject>* nqdObj = nullptr;
	nqdObj = testQ.enqueue(new TestingObject(NQDOBJ_VALUE));

	if(nqdObj != nullptr && nqdObj->getData()->getData() == NQDOBJ_VALUE && testQ.empty() == false && testQ.peek()->getData()->getData() == NQDOBJ_VALUE) {
		return 0;
	} else {
		return 1;
	}
}


int test_queue_one_child_count() {
	Queue<TestingObject> testQ;
	testQ.enqueue(new TestingObject(69));

	if(testQ.length() == 1) {
		return 0;
	} else {
		return 1;
	}
}


int test_queue_two_children_count() {
	Queue<TestingObject> testQ;
	testQ.enqueue(new TestingObject(69));
	testQ.enqueue(new TestingObject(1337));

	if(testQ.length() == 2) {
		return 0;
	} else {
		return 1;
	}
}


int test_queue_enqueue_dequeue_order() {
	const unsigned short ITERATIONS = 3;

	Queue<TestingObject> testQ;

	for(int i = 0; i < ITERATIONS; i += 1) {
		testQ.enqueue(new TestingObject(i));
	}

	if(testQ.length() != ITERATIONS) {
		return 1;
	}

	QueueNode<TestingObject>* tempObj = nullptr;
	for(int i = 0; i < ITERATIONS; i += 1) {
		tempObj = testQ.dequeue();
		if(tempObj->getData()->getData() != i) {
			return 2;
		}
		delete tempObj;
	}

	if(testQ.dequeue() != nullptr) {
		return 4;
	}

	return 0;
}


int test_queue_dequeue_empty() {
	Queue<TestingObject> testQ;
	if(testQ.dequeue() == nullptr) {
		return 0;
	} else {
		return 1;
	}
}


int test_queue_dequeue_last() {
	Queue<TestingObject> testQ;
	testQ.enqueue(new TestingObject(69));

	QueueNode<TestingObject>* retObj = nullptr;
	retObj = testQ.dequeue();
	if(retObj != nullptr && testQ.peek() == nullptr) {
		delete retObj;
		return 0;
	} else {
		delete retObj;
		return 1;
	}
}


int test_queue_dequeue_not_last() {
	const unsigned short REMAINING_OBJECT_VALUE = 96;

	Queue<TestingObject> testQ;
	testQ.enqueue(new TestingObject(69));
	testQ.enqueue(new TestingObject(REMAINING_OBJECT_VALUE));

	QueueNode<TestingObject>* retObj = nullptr;
	retObj = testQ.dequeue();
	if(retObj != nullptr && testQ.peek() != nullptr && testQ.peek()->getData()->getData() == REMAINING_OBJECT_VALUE) {
		delete retObj;
		return 0;
	} else {
		delete retObj;
		return 1;
	}
}


int test_queue_enqueue_nullptr() {
	Queue<TestingObject> testQ;
	if(testQ.enqueue(nullptr) == nullptr && testQ.empty() == true) {
		return 0;
	} else {
		return 1;
	}
}


int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "No test ID provided! Expecting unsigned int" << std::endl;
		return 1;
	}

	const unsigned short TEST_ID = atoi(argv[1]);
	switch(TEST_ID) {
	case 0:
		return test_queue_no_children_count();
	break;

	case 1:
		return test_queue_add_node();
	break;

	case 2:
		return test_queue_one_child_count();
	break;

	case 3:
		return test_queue_two_children_count();
	break;

	case 4:
		return test_queue_enqueue_dequeue_order();
	break;

	case 5:
		return test_queue_dequeue_empty();
	break;

	case 6:
		return test_queue_dequeue_last();
	break;

	case 7:
		return test_queue_dequeue_not_last();
	break;

	case 8:
		return test_queue_enqueue_nullptr();
	break;

	default:
		std::cout << "Unknown test ID!" << std::endl;
		return 1;
	break;
	}

	return 0;
}

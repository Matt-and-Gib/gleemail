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
	Queue<TestingObject> testQ;
	testQ.enqueue(new TestingObject(69));

	if(testQ.empty() == false) {
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

	default:
		std::cout << "Unknown test ID!" << std::endl;
		return 1;
	break;
	}

	return 0;
}

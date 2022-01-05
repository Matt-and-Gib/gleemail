#include "queue.h"
#include <iostream>
#include <stdlib.h>


class TestingObject {
private:
	int data = -1;
public:
	TestingObject(const int d) : data{d} {}
};


int test_queue_no_children_count() {
	Queue<TestingObject> testQ;

	if((testQ.empty() == true) && (testQ.length() == 0)) {
		return 0;
	} else {
		return 1;
	}
}


int test_queue_add_node() {
	Queue<TestingObject> testQ;

	TestingObject* testD = new TestingObject(69);
	testQ.enqueue(testD);

	if(testQ.empty() == false) {
		return 0;
	} else {
		return 1;
	}
}


int test_queue_one_child_count() {
	Queue<TestingObject> testQ;

	TestingObject* testD = new TestingObject(69);
	testQ.enqueue(testD);

	if((testQ.empty() == false) && (testQ.length() == 1)) {
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

	default:
		std::cout << "Unknown test ID!" << std::endl;
		return 1;
	break;
	}

	return 0;
}

#include "binarysearchtree.h"
#include <iostream>
#include <stdlib.h>


class TestingObject {
private:
	char data = 0;
public:
	TestingObject(const char d) : data{d} {}
	char getData() {return data;}

	bool operator==(const TestingObject& o) const {
		return data == o.data;
	}
};


int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "No test ID provided! Expecting unsigned int" << std::endl;
		return 1;
	}

	const unsigned short TEST_ID = atoi(argv[1]);
	switch(TEST_ID) {
	case 0:
		return 0; //test_bstree_();
	break;

	default:
		std::cout << "Unknown test ID!" << std::endl;
		return 1;
	break;
	}

	return 0;
}

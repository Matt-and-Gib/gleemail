#include "debuglog.h"
#include <iostream>


int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "No test ID provided! Expecting unsigned int" << std::endl;
		return 1;
	}

	const unsigned short TEST_ID = atoi(argv[1]);
	switch(TEST_ID) {

	default:
		std::cout << "Unknown test ID!" << std::endl;
		return 1;
	break;
	}

	return 0;
}

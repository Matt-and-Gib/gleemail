#include "queue.h"
#include <iostream>

#ifdef UNIT_TEST_MODE
#define value "SUCCESS"
#else
#define value "FAILURE"
#endif

int main(int argc, char* argv[]) {
	std::cout << value;

	return 0;
}

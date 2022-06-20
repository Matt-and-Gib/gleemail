#include "keyvaluepair.h"
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


int test_kvp_different_types() {
	int k = 69;
	char p = 'x';
	KVPair<int, char> testKVP(k, p);

	if(testKVP.getKey() == k && testKVP.getValue() == p) {
		return 0;
	} else {
		return 1;
	}
}


int test_kvp_same_type() {
	int k = 69;
	int p = 96;
	KVPair<int, char> testKVP(k, p);

	if(testKVP.getKey() == k && testKVP.getValue() == p) {
		return 0;
	} else {
		return 1;
	}
}


int test_kvp_same_object() {
	int v = 69;
	KVPair<int, char> testKVP(v, v);

	if(testKVP.getKey() == v && testKVP.getValue() == v) {
		return 0;
	} else {
		return 1;
	}
}


int test_kvp_pointer_type() {
	int k = 69;
	TestingObject* v = new TestingObject('x');
	KVPair<int, TestingObject*> testKVP(k, v);

	if(testKVP.getKey() == k && testKVP.getValue() == v && testKVP.getValue()->getData() == v->getData()) {
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
		return test_kvp_different_types();
	break;
	
	case 1:
		return test_kvp_same_type();
	break;

	case 2:
		return test_kvp_same_object();
	break;

	case 3:
		return test_kvp_pointer_type();
	break;

	default:
		std::cout << "Unknown test ID!" << std::endl;
		return 1;
	break;
	}

	return 0;
}

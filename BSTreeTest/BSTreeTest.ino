#include "Arduino.h"
#include "binarysearchtree.h"


struct ArrObj {
	ArrObj(const char v = '\0') {value = v;}

	bool operator==(const ArrObj& o) {return value == o.value;}
	bool operator!=(const ArrObj& o) {return value != o.value;}
	bool operator>(const ArrObj& o) {
		if(value == o.value) return false;
		return (value == '-');
	};
	bool operator<(const ArrObj& o) {
		if(value == o.value) return false;
		return (value == '.');
	};

	char value;
};


struct CustObj {
	CustObj() {val = new ArrObj[VAL_ARR_LEN];}

	ArrObj& lastVal() {return val[firstOpenIndex - 1];}

	void push(char o) {
		if(firstOpenIndex == VAL_ARR_LEN) {
			return;
		}

		val[firstOpenIndex++] = *new ArrObj(o);
	}

	void printString() {
		for(int i = 0; i < VAL_ARR_LEN; i += 1) {
			if(val[i].value == '\0') {
				break;
			}

			Serial.print(val[i].value);
		}
	}

	ArrObj *val;
	const unsigned short VAL_ARR_LEN = 6;
	unsigned short firstOpenIndex = 0;
};


struct CustPair {
	CustPair(CustObj* k, char v) {key = k; value = v;}
	CustObj* key;
	char value;

	//if a < b
	bool operator<(CustPair& o) {
		//return this->key->lastVal() < o.key->lastVal();
		for(int i = 0; i < key->VAL_ARR_LEN; i += 1) {
			Serial.println(i);
			Serial.println(o.value);
			/*Serial.flush();
			delay(250);
			if((*key).val[i] > o.key->val[i]) {
				return false;
			}*/
		}

		return true;
	}

	bool operator==(CustPair& o) {
		return value == o.value;
	}

	void printString() {
		key->printString();
		Serial.print(" : ");
		Serial.println(value);
	}
};


void basicTests() {
	CustObj sl;
	sl.push('.');
	sl.push('-');
	CustPair a(&sl, 'a');
	//a.printString();

	CustObj s;
	s.push('.');
	CustPair e(&s, 'e');
	//e.printString();

	CustObj ss;
	ss.push('.');
	ss.push('.');
	CustPair i(&ss, 'i');
	//i.printString();

	CustObj ls;
	ls.push('-');
	ls.push('.');
	CustPair n(&ls, 'n');
	//n.printString();

	CustObj l;
	l.push('-');
	CustPair t(&l, 't');
	//t.printString();

	CustObj ll;
	ll.push('-');
	ll.push('-');
	CustPair m(&ll, 'm');
	//m.printString();

	CustObj r;
	CustPair root(&r, '\0');

	BinarySearchTreeNode<CustPair> lookupTree(&root, nullptr);
	lookupTree.insert(&e);
	lookupTree.insert(&a);
	lookupTree.insert(&i);
	lookupTree.insert(&n);

	lookupTree.print();
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	Serial.println("Begin basic tests");
	basicTests();
	Serial.println("Done with basic tests");
}


void loop() {

}
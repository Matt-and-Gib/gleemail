#include "Arduino.h"
#include "binarysearchtree.h"


struct ArrObj {
	ArrObj() {value = '\0';}
	ArrObj(const char v) {value = v;}

	bool operator==(const ArrObj& o) {return value == o.value;}
	bool operator!=(const ArrObj& o) {return value != o.value;}
	bool operator>(const ArrObj& o) {
		if(value == o.value) return false;
		if(o.value == '\0') return true;
		return (value == '-');
	};
	bool operator<(const ArrObj& o) {
		if(value == o.value) return false;
		if(value == '\0') return true;
		return (value == '.');
	};

	char value;
};


struct CustObj {
	CustObj() {val = new ArrObj[VAL_ARR_LEN];}

	/*void push(ArrObj &o) {
		if(firstOpenIndex == VAL_ARR_LEN) {
			return;
		}

		val[firstOpenIndex++] = o;
	}*/
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
	a.printString();

	CustObj s;
	s.push('.');
	CustPair e(&s, 'e');
	e.printString();

	CustObj ss;
	ss.push('.');
	ss.push('.');
	CustPair i(&ss, 'i');
	i.printString();

	CustObj ls;
	ls.push('-');
	ls.push('.');
	CustPair n(&ls, 'n');
	n.printString();

	CustObj l;
	l.push('-');
	CustPair t(&l, 't');
	t.printString();

	CustObj ll;
	ll.push('-');
	ll.push('-');
	CustPair m(&ll, 'm');
	m.printString();

	//BinarySearchTree<CustPair> lookupTree;
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	basicTests();
}


void loop() {

}
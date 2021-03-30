#include "Arduino.h"
#include "morsecodetree.h"


void basicTests() {
	CustObj r;
	CustPair root(&r, '\0');
	MorseCodeTreeNode morseCodeTreeRootNode(&root, nullptr);

	CustObj temp_s;
	temp_s.push('.');
	CustPair e(&temp_s, 'e');
	morseCodeTreeRootNode.insert(&e);

	CustObj temp_l;
	temp_l.push('-');
	CustPair t(&temp_l, 't');
	morseCodeTreeRootNode.insert(&t);

	CustObj ss;
	ss.push('.');
	ss.push('.');
	CustPair i(&ss, 'i');
	morseCodeTreeRootNode.insert(&i);

	CustObj sl;
	sl.push('.');
	sl.push('-');
	CustPair a(&sl, 'a');
	morseCodeTreeRootNode.insert(&a);

	CustObj ls;
	ls.push('-');
	ls.push('.');
	CustPair n(&ls, 'n');
	morseCodeTreeRootNode.insert(&n);

	CustObj ll;
	ll.push('-');
	ll.push('-');
	CustPair m(&ll, 'm');
	morseCodeTreeRootNode.insert(&m);

	CustObj sss;
	sss.push('.');
	sss.push('.');
	sss.push('.');
	CustPair s(&sss, 's');
	morseCodeTreeRootNode.insert(&s);

	CustObj ssl;
	ssl.push('.');
	ssl.push('.');
	ssl.push('-');
	CustPair u(&ssl, 'u');
	morseCodeTreeRootNode.insert(&u);
	
/*	complete above this line	*/

	
































































































/*
	CustObj lllls;
	lllls.push('-');
	lllls.push('-');
	lllls.push('-');
	lllls.push('-');
	lllls.push('.');
	CustPair nine(&lllls, '0');
	morseCodeTreeRootNode.insert(&nine);

	CustObj lllll;
	lllll.push('-');
	lllll.push('-');
	lllll.push('-');
	lllll.push('-');
	lllll.push('-');
	CustPair zero(&lllll, '0');
	morseCodeTreeRootNode.insert(&zero);
*/
	morseCodeTreeRootNode.print();
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
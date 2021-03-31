#include "Arduino.h"
#include "morsecodetree.h"


void basicTests() {
	CustObj rootObj;
	CustPair root(&rootObj, '\0');
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

	CustObj sls;
	sls.push('.');
	sls.push('-');
	sls.push('.');
	CustPair r(&sls, 'r');
	morseCodeTreeRootNode.insert(&r);

	CustObj sll;
	sll.push('.');
	sll.push('-');
	sll.push('-');
	CustPair w(&sll, 'w');
	morseCodeTreeRootNode.insert(&w);

	CustObj lss;
	lss.push('-');
	lss.push('.');
	lss.push('.');
	CustPair d(&lss, 'd');
	morseCodeTreeRootNode.insert(&d);

	CustObj lsl;
	lsl.push('-');
	lsl.push('.');
	lsl.push('-');
	CustPair k(&lsl, 'k');
	morseCodeTreeRootNode.insert(&k);

	CustObj lls;
	lls.push('l');
	lls.push('-');
	lls.push('.');
	CustPair g(&lls, 'g');
	morseCodeTreeRootNode.insert(&g);

	CustObj lll;
	lll.push('-');
	lll.push('-');
	lll.push('-');
	CustPair o(&lll, 'o');
	morseCodeTreeRootNode.insert(&o);

	CustObj ssss;
	ssss.push('.');
	ssss.push('.');
	ssss.push('.');
	ssss.push('.');
	CustPair h(&ssss, 'h');
	morseCodeTreeRootNode.insert(&h);

	CustObj sssl;
	sssl.push('.');
	sssl.push('.');
	sssl.push('.');
	sssl.push('-');
	CustPair v(&sssl, 'v');
	morseCodeTreeRootNode.insert(&v);

	CustObj ssls;
	ssls.push('.');
	ssls.push('.');
	ssls.push('-');
	ssls.push('.');
	CustPair f(&ssls, 'f');
	morseCodeTreeRootNode.insert(&f);

	CustObj ssll;
	ssll.push('.');
	ssll.push('.');
	ssll.push('-');
	ssll.push('-');
	CustPair placeholder_ssll(&ssll, '\0');
	morseCodeTreeRootNode.insert(&placeholder_ssll);

	CustObj slss;
	slss.push('.');
	slss.push('-');
	slss.push('.');
	slss.push('.');
	CustPair l(&slss, 'l');
	morseCodeTreeRootNode.insert(&l);

	CustObj slsl;
	slsl.push('.');
	slsl.push('-');
	slsl.push('.');
	slsl.push('-');
	CustPair placeholder_slsl(&sls, '\0');
	morseCodeTreeRootNode.insert(&placeholder_slsl);

	CustObj slls;
	slls.push('.');
	slls.push('-');
	slls.push('-');
	slls.push('.');
	CustPair p(&slls, 'p');
	morseCodeTreeRootNode.insert(&p);

	CustObj slll;
	slll.push('.');
	slll.push('-');
	slll.push('-');
	slll.push('-');
	CustPair j(&slll, 'j');
	morseCodeTreeRootNode.insert(&j);

	CustObj lsss;
	lsss.push('-');
	lsss.push('.');
	lsss.push('.');
	lsss.push('.');
	CustPair b(&lsss, 'b');
	morseCodeTreeRootNode.insert(&b);

	CustObj lssl;
	lssl.push('-');
	lssl.push('.');
	lssl.push('.');
	lssl.push('-');
	CustPair x(&lssl, 'x');
	morseCodeTreeRootNode.insert(&x);

	CustObj lsls;
	lsls.push('-');
	lsls.push('.');
	lsls.push('-');
	lsls.push('.');
	CustPair c(&lsls, 'c');
	morseCodeTreeRootNode.insert(&c);

	CustObj lsll;
	lsll.push('-');
	lsll.push('.');
	lsll.push('-');
	lsll.push('-');
	CustPair y(&lsll, 'y');
	morseCodeTreeRootNode.insert(&y);

	CustObj llss;
	llss.push('-');
	llss.push('-');
	llss.push('.');
	llss.push('.');
	CustPair z(&llss, 'z');
	morseCodeTreeRootNode.insert(&z);

	CustObj llsl;
	llsl.push('-');
	llsl.push('-');
	llsl.push('.');
	llsl.push('-');
	CustPair q(&llsl, 'q');
	morseCodeTreeRootNode.insert(&q);

	CustObj llls;
	llls.push('-');
	llls.push('-');
	llls.push('-');
	llls.push('.');
	CustPair placeholder_llls(&llls, '\0');
	morseCodeTreeRootNode.insert(&placeholder_llls);

	CustObj llll;
	llll.push('-');
	llll.push('-');
	llll.push('-');
	llll.push('-');
	CustPair placeholder_llll(&llll, '\0');
	morseCodeTreeRootNode.insert(&placeholder_llll);

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
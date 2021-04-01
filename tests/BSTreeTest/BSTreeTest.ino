#include "Arduino.h"
#include "morsecodetree.h"

/*
	Timing Notes
		Complex Compare:
			Creating and Inserting all nodes took ~2201 microseconds (~2 milliseconds)
			Printing tree took ~61294 microseconds (~61 milliseconds)
		Simple Compare:
			Creating and Inserting all nodes took ~2216 microseconds (~2 milliseconds)
			Printing tree took ~59235 microseconds (~59 milliseconds)
*/


void basicTests() {
	Serial.print ("begin creating and inserting nodes at ");
	Serial.println(micros());

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
	CustPair placeholder_ssll(&ssll, ' ');
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
	CustPair placeholder_slsl(&sls, ' ');
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
	CustPair placeholder_llls(&llls, ' ');
	morseCodeTreeRootNode.insert(&placeholder_llls);

	CustObj llll;
	llll.push('-');
	llll.push('-');
	llll.push('-');
	llll.push('-');
	CustPair placeholder_llll(&llll, ' ');
	morseCodeTreeRootNode.insert(&placeholder_llll);

	CustObj sssss;
	sssss.push('.');
	sssss.push('.');
	sssss.push('.');
	sssss.push('.');
	sssss.push('.');
	CustPair five(&sssss, '5');
	morseCodeTreeRootNode.insert(&five);

	CustObj ssssl;
	ssssl.push('.');
	ssssl.push('.');
	ssssl.push('.');
	ssssl.push('.');
	ssssl.push('-');
	CustPair four(&ssssl, '4');
	morseCodeTreeRootNode.insert(&four);

	CustObj sssls;
	sssls.push('.');
	sssls.push('.');
	sssls.push('.');
	sssls.push('-');
	sssls.push('.');
	CustPair placeholder_sssls(&sssls, ' ');
	morseCodeTreeRootNode.insert(&placeholder_sssls);

	CustObj sssll;
	sssll.push('.');
	sssll.push('.');
	sssll.push('.');
	sssll.push('-');
	sssll.push('-');
	CustPair three(&sssll, '3');
	morseCodeTreeRootNode.insert(&three);

	CustObj sslss;
	sslss.push('.');
	sslss.push('.');
	sslss.push('-');
	sslss.push('.');
	sslss.push('.');
	CustPair placeholder_sslss(&sslss, ' ');
	morseCodeTreeRootNode.insert(&placeholder_sslss);

	CustObj sslsl;
	sslsl.push('.');
	sslsl.push('.');
	sslsl.push('-');
	sslsl.push('.');
	sslsl.push('-');
	CustPair placeholder_sslsl(&sslsl, ' ');
	morseCodeTreeRootNode.insert(&placeholder_sslsl);

	CustObj sslls;
	sslls.push('.');
	sslls.push('.');
	sslls.push('-');
	sslls.push('-');
	sslls.push('.');
	CustPair placeholder_sslls(&sslls, ' ');
	morseCodeTreeRootNode.insert(&placeholder_sslls);

	CustObj sslll;
	sslll.push('.');
	sslll.push('.');
	sslll.push('-');
	sslll.push('-');
	sslll.push('-');
	CustPair two(&sslll, '2');
	morseCodeTreeRootNode.insert(&two);

	CustObj slsss;
	slsss.push('.');
	slsss.push('-');
	slsss.push('.');
	slsss.push('.');
	slsss.push('.');
	CustPair ampersand(&slsss, '&');
	morseCodeTreeRootNode.insert(&ampersand);

	CustObj slssl;
	slssl.push('.');
	slssl.push('-');
	slssl.push('.');
	slssl.push('.');
	slssl.push('-');
	CustPair placeholder_slssl(&slssl, ' ');
	morseCodeTreeRootNode.insert(&placeholder_slssl);

	CustObj slsls;
	slsls.push('.');
	slsls.push('-');
	slsls.push('.');
	slsls.push('-');
	slsls.push('.');
	CustPair plus(&slsls, '+');
	morseCodeTreeRootNode.insert(&plus);

	CustObj slsll;
	slsll.push('.');
	slsll.push('-');
	slsll.push('.');
	slsll.push('-');
	slsll.push('-');
	CustPair placeholder_slsll(&slsll, ' ');
	morseCodeTreeRootNode.insert(&placeholder_slsll);

	CustObj sllss;
	sllss.push('.');
	sllss.push('-');
	sllss.push('-');
	sllss.push('.');
	sllss.push('.');
	CustPair placeholder_sllss(&sllss, ' ');
	morseCodeTreeRootNode.insert(&placeholder_sllss);
	
	CustObj sllsl;
	sllsl.push('.');
	sllsl.push('-');
	sllsl.push('-');
	sllsl.push('.');
	sllsl.push('-');
	CustPair placeholder_sllsl(&sllsl, ' ');
	morseCodeTreeRootNode.insert(&placeholder_sllsl);

	CustObj sllls;
	sllls.push('.');
	sllls.push('-');
	sllls.push('-');
	sllls.push('-');
	sllls.push('.');
	CustPair placeholder_sllls(&sllls, ' ');
	morseCodeTreeRootNode.insert(&placeholder_sllls);

	CustObj sllll;
	sllll.push('.');
	sllll.push('-');
	sllll.push('-');
	sllll.push('-');
	sllll.push('-');
	CustPair one(&sllll, '1');
	morseCodeTreeRootNode.insert(&one);

	CustObj lssss;
	lssss.push('-');
	lssss.push('.');
	lssss.push('.');
	lssss.push('.');
	lssss.push('.');
	CustPair six(&lssss, '6');
	morseCodeTreeRootNode.insert(&six);

	CustObj lsssl;
	lsssl.push('-');
	lsssl.push('.');
	lsssl.push('.');
	lsssl.push('.');
	lsssl.push('-');
	CustPair equals(&lsssl, '=');
	morseCodeTreeRootNode.insert(&equals);

	CustObj lssls;
	lssls.push('-');
	lssls.push('.');
	lssls.push('.');
	lssls.push('-');
	lssls.push('.');
	CustPair forwardSlash(&lssls, '/');
	morseCodeTreeRootNode.insert(&forwardSlash);

	CustObj lssll;
	lssll.push('-');
	lssll.push('.');
	lssll.push('.');
	lssll.push('-');
	lssll.push('-');
	CustPair placeholder_lssll(&lssll, ' ');
	morseCodeTreeRootNode.insert(&placeholder_lssll);

	CustObj lslss;
	lslss.push('-');
	lslss.push('.');
	lslss.push('-');
	lslss.push('.');
	lslss.push('.');
	CustPair placeholder_lslss(&lslss, ' ');
	morseCodeTreeRootNode.insert(&placeholder_lslss);

	CustObj lslsl;
	lslsl.push('-');
	lslsl.push('.');
	lslsl.push('-');
	lslsl.push('.');
	lslsl.push('-');
	CustPair placeholder_lslsl(&lslsl, ' ');
	morseCodeTreeRootNode.insert(&placeholder_lslsl);

	CustObj lslls;
	lslls.push('-');
	lslls.push('.');
	lslls.push('-');
	lslls.push('-');
	lslls.push('.');
	CustPair parenthesisOpen(&lslls, '(');
	morseCodeTreeRootNode.insert(&parenthesisOpen);

	CustObj lslll;
	lslll.push('-');
	lslll.push('.');
	lslll.push('-');
	lslll.push('-');
	lslll.push('-');
	CustPair placeholder_lslll(&lslll, ' ');
	morseCodeTreeRootNode.insert(&placeholder_lslll);

	CustObj llsss;
	llsss.push('-');
	llsss.push('-');
	llsss.push('.');
	llsss.push('.');
	llsss.push('.');
	CustPair seven(&llsss, '7');
	morseCodeTreeRootNode.insert(&seven);

	CustObj llssl;
	llssl.push('-');
	llssl.push('-');
	llssl.push('.');
	llssl.push('.');
	llssl.push('-');
	CustPair placeholder_llssl(&llssl, ' ');
	morseCodeTreeRootNode.insert(&placeholder_llssl);

	CustObj llsls;
	llsls.push('-');
	llsls.push('-');
	llsls.push('.');
	llsls.push('-');
	llsls.push('.');
	CustPair placeholder_llsls(&llsls, ' ');
	morseCodeTreeRootNode.insert(&placeholder_llsls);

	CustObj llsll;
	llsll.push('-');
	llsll.push('-');
	llsll.push('.');
	llsll.push('-');
	llsll.push('-');
	CustPair placeholder_llsll(&llsll, ' ');
	morseCodeTreeRootNode.insert(&placeholder_llsll);

	CustObj lllss;
	lllss.push('-');
	lllss.push('-');
	lllss.push('-');
	lllss.push('.');
	lllss.push('.');
	CustPair eight(&lllss, '8');
	morseCodeTreeRootNode.insert(&eight);

	CustObj lllsl;
	lllsl.push('-');
	lllsl.push('-');
	lllsl.push('-');
	lllsl.push('.');
	lllsl.push('-');
	CustPair placeholder_lllsl(&lllsl, ' ');
	morseCodeTreeRootNode.insert(&placeholder_lllsl);

	CustObj lllls;
	lllls.push('-');
	lllls.push('-');
	lllls.push('-');
	lllls.push('-');
	lllls.push('.');
	CustPair nine(&lllls, '9');
	morseCodeTreeRootNode.insert(&nine);

	CustObj lllll;
	lllll.push('-');
	lllll.push('-');
	lllll.push('-');
	lllll.push('-');
	lllll.push('-');
	CustPair zero(&lllll, '0');
	morseCodeTreeRootNode.insert(&zero);

	Serial.print ("done creating and inserting nodes at ");
	Serial.println(micros());

	Serial.print ("begin printing tree at ");
	Serial.println(micros());

	morseCodeTreeRootNode.print();

	Serial.print ("done printing tree at ");
	Serial.println(micros());

	CustObj findMe;
	findMe.push('-');
	findMe.push('.');
	findMe.push('.');

	CustPair* foundYou = morseCodeTreeRootNode.lookup(findMe);
	if(foundYou) {
		Serial.println(foundYou->value);
	} else {
		Serial.println("No key found");
	}
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
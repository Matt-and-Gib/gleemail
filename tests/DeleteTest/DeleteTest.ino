#include "Arduino.h"
#include "HardwareSerial.h"


class IDT {
private:
	int v;
public:
	IDT() {v = 1337;}
};


void setup() {
	Serial.begin(9600);
	while(!Serial){
		delay(250);
	}

	IDT* testy = new IDT();
	Serial.println("delete");
	delete testy;

	IDT* testy2 = new IDT();
	Serial.println("delete[]");
	delete[] testy2;

	Serial.println("Done");
}


void loop() {

}
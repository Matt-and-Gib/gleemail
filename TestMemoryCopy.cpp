//#include "Arduino.h"
#include "HardwareSerial.h"


void setup() {
	static const unsigned short ARRAY_LENGTH = 16;
	char arrayOne[ARRAY_LENGTH] = {'a'};
	char arrayTwo[ARRAY_LENGTH] = {'b'};

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	Serial.print("item 0 of array 1 = ");
	Serial.println(arrayOne[0]);

	Serial.print("item 0 of array 2 = ");
	Serial.println(arrayTwo[0]);

	for(int i = 0; i < ARRAY_LENGTH; i += 1) {
		arrayTwo[i] = arrayOne[i];
		arrayOne[i] = 'c';
	}

	Serial.print("item 0 of array 2 = ");
	Serial.println(arrayTwo[0]);

	Serial.print("item 0 of array 1 = ");
	Serial.println(arrayOne[0]);
}


void loop() {

}
#include "Arduino.h"


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}
}


void loop() {

}
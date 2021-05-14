#include "Arduino.h"
#include "Display.h"


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	Display* d = new Display();

	const unsigned long startTime = micros();
	d->updateReading("................");
	const unsigned long endTime = micros();

	Serial.print("took: ");
	Serial.println(endTime - startTime); //1036
}


void loop() {

}
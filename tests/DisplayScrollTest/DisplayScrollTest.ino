#include "Arduino.h"
//#include "../../src/include/display.h"

#include <hd44780.h>
#include <hd44780ioClass/hd44780_pinIO.h>


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	unsigned short RS = 7;
	unsigned short EN = 6;
	unsigned short D4 = 5;
	unsigned short D5 = 4;
	unsigned short D6 = 3;
	unsigned short D7 = 2;

	hd44780_pinIO* lcd = new hd44780_pinIO(RS, EN, D4, D5, D6, D7);
	lcd->begin(16, 2);

	const char longMessage[] = "This message is longer than forty characters, so I don't know what will happen. Now it's way too long!";

	Serial.println(F("printing"));

	lcd->setCursor(0, 0);
	lcd->print(longMessage);
	
	Serial.println(F("done"));
}


void loop() {

}
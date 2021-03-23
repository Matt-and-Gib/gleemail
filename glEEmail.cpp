#include "Arduino.h"
#include "HardwareSerial.h"
//#include <SPI.h>
//#include <WiFiNINA.h>

#include "morsecode.h"


constexpr unsigned short BAUD_RATE = 9600;

static MorseCodeInput morseCode;


void loop () {
	switch(digitalRead(morseCode.getSwitchPin())) {
	case HIGH:
		digitalWrite(morseCode.getLEDPin(), HIGH);
		morseCode.pushCharacter(true);
		break;

	case LOW:
		digitalWrite(morseCode.getLEDPin(), LOW);
		morseCode.pushCharacter();
		break;
	}
}


void setup() {
	pinMode(morseCode.getSwitchPin(), INPUT);
	pinMode(morseCode.getLEDPin(), OUTPUT);

	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	Serial.println("Running");
}
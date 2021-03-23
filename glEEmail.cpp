#include "Arduino.h"
#include "HardwareSerial.h"
//#include <SPI.h>
//#include <WiFiNINA.h>

#include "morsecode.h"


constexpr unsigned short BAUD_RATE = 9600;

static InputDevice *morseCode = new MorseCodeInput();


void loop () {
	switch(digitalRead(morseCode.getSwitchPin())) {
	case HIGH:
		digitalWrite(morseCode->getLEDPin(), HIGH);
		morseCode->pushCharacter(true);
		break;

	case LOW:
		digitalWrite(morseCode->getLEDPin(), LOW);
		morseCode->pushCharacter();
		break;
	}
}


void setupPins() {
	Pin *pins = morseCode->getPins();
	unsigned short i = 0;
	Pin currentPin = pins[i];
	while (currentPin != NULL_PIN) {
		pinMode(currentPin.index, currentPin.mode);
		currentPin = pins[++i];
	}
}


void setup() {
	setupPins();

	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	Serial.println("Running");
}
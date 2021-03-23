#include "Arduino.h"
#include "HardwareSerial.h"
//#include <SPI.h>
//#include <WiFiNINA.h>

#include "morsecode.h"


constexpr unsigned short BAUD_RATE = 9600;

static InputMethod *input = new MorseCodeInput();


void loop () {
	/*switch(digitalRead(input.getSwitchPin())) {
	case HIGH:
		digitalWrite(input->getLEDPin(), HIGH);
		input->pushCharacter(true);
		break;

	case LOW:
		digitalWrite(input->getLEDPin(), LOW);
		input->pushCharacter();
		break;
	}*/
}


void setupPins() {
	const Pin *pins = input->getPins();
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
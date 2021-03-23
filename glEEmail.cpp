#include "Arduino.h"
#include "HardwareSerial.h"
//#include <SPI.h>
//#include <WiFiNINA.h>

#include "morsecode.h"


static constexpr unsigned short BAUD_RATE = 9600;

/*
	SWITCH_PIN_INDEX assign digital pin index for swtich used with your Arduino board
	LED_PIN_INDEX assign digital pin index for LED used with your Arduino board
*/
static constexpr unsigned short SWITCH_PIN_INDEX = 2;//4;
//static constexpr unsigned short LED_PIN_INDEX = 5;


static InputMethod *input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN); //LED_PIN_INDEX);

static unsigned short pinIndex = 0;

void loop () {
	pinIndex = 0;

	Pin **allPins = input->getPins();
	Pin *currentPin = allPins[pinIndex];
	while(*currentPin != *NULL_PIN) {
		if(currentPin->mode == PIN_MODE::READ) {
			currentPin->value = digitalRead(currentPin->pinLocation);
		}

		currentPin = allPins[++pinIndex];
	}

	input->processInput(millis());

	pinIndex = 0;
	currentPin = allPins[pinIndex];
	while(*currentPin != *NULL_PIN) {
		if(currentPin->mode == PIN_MODE::WRITE) {
			digitalWrite(currentPin->pinLocation, currentPin->value);
		}

		currentPin = allPins[++pinIndex];
	}
}


void setupPins() {
	Pin **pins = input->getPins();
	unsigned short i = 0;
	Pin *currentPin = pins[i];

	while (*currentPin != *NULL_PIN) {
		pinMode(currentPin->pinLocation, currentPin->mode);
		currentPin = pins[++i];
	}
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	setupPins();

	Serial.println("Running");
}
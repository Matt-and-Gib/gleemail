#include "Arduino.h"
//#include "HardwareSerial.h"
//#include <SPI.h>
//#include <WiFiNINA.h>

#include "morsecode.h"


static constexpr unsigned short BAUD_RATE = 9600;

static constexpr unsigned short SWITCH_PIN_INDEX = 4;

static InputMethod *input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN);

static unsigned short pinIndex = 0;


void loop () {
	pinIndex = 0;

	Pin **allPins = input->getPins();
	Pin *currentPin = allPins[pinIndex];
	while(*currentPin != NULL_PIN) {
		if(currentPin->mode == PIN_MODE::READ) {
			currentPin->value = digitalRead(currentPin->pinLocation);
		}

		currentPin = allPins[++pinIndex];
	}

	input->processInput(millis());

	pinIndex = 0;
	currentPin = allPins[pinIndex];
	while(*currentPin != NULL_PIN) {
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

	while (*currentPin != NULL_PIN) {
		Serial.print("Index: ");
		Serial.println(i);

		Serial.print(currentPin->pinLocation);
		Serial.print(" : ");
		Serial.println(currentPin->mode);

		pinMode(currentPin->pinLocation, currentPin->mode);
		currentPin = pins[++i];
	}
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	Serial.println("Serial setup");

	setupPins();

	Serial.println("Running");
}
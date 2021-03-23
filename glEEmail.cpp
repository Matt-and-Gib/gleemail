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
static constexpr unsigned short SWITCH_PIN_INDEX = 4;
static constexpr unsigned short LED_PIN_INDEX = 5; //LED_BUILTIN;


static InputMethod *input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_PIN_INDEX);

static unsigned short pinIndex = 0;

void loop () {
	pinIndex = 0;

	Pin **readPins = input->getReadPins();
	Pin *currentPin = readPins[pinIndex];

	Serial.print("readPin 0 pin location: ");
	Serial.println(currentPin->pinLocation);

	while (*currentPin != *NULL_PIN) {
		currentPin->value = digitalRead(currentPin->pinLocation);
		currentPin = readPins[++pinIndex];
	}

	input->processInput(millis());

	pinIndex = 0;

	Pin **writePins = input->getWritePins();
	currentPin = writePins[pinIndex];
	while(*currentPin != *NULL_PIN) {
		digitalWrite(currentPin->pinLocation, currentPin->value);
		currentPin = writePins[++pinIndex];
	}
	
}


void setupPins() {
	Pin **pins = input->getPins();
	unsigned short i = 0;
	Pin *currentPin = pins[i];

	while (*currentPin != *NULL_PIN) {
		Serial.print("pin ");
		Serial.print(i);
		Serial.print(" location: ");
		Serial.println(currentPin->pinLocation);

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

	/*Pin **pins = input->getPins();
	Pin &testPin = *pins[0];
	Serial.print("getPins 0 location: ");
	Serial.println(testPin.pinLocation);*/

	Serial.println("Running");
}
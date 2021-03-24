#include "Arduino.h"
//#include "HardwareSerial.h"
//#include <SPI.h>
//#include <WiFiNINA.h>

#include "global.h"
#include "morsecode.h"


static unsigned short pinIndex = 0;
static InputMethod *input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN);
static char messageOut[MAX_MESSAGE_LENGTH] = {'\0'};


void processInputMethod() {
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


void loop() {
	processInputMethod();
	if(input->isMessageReady()) {
		input->getMessageToSend(messageOut);
		//sendMessage(inputMethod->getMessageToSend());
	}
	//messageIn = receiveMessage();
	//printMessage(); //write to LCD buffer, I guess
}


void setupPins() {
	Pin **pins = input->getPins();
	unsigned short i = 0;
	Pin *currentPin = pins[i];

	while (*currentPin != NULL_PIN) {
		/*Important debug messages. Check here first if something seems broken!
		Serial.print("Index: ");
		Serial.println(i);

		Serial.print(currentPin->pinLocation);
		Serial.print(" : ");
		Serial.println(currentPin->mode);*/

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
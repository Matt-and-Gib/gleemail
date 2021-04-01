#include "Arduino.h"
//#include "HardwareSerial.h"
//#include <SPI.h>
//#include <WiFiNINA.h>

#include "src/include/global.h"
#include "src/include/morsecode.h"


static unsigned short pinIndex = 0;
static unsigned short errorIndex = 0;
static InputMethod *input = new MorseCodeInput(SWITCH_PIN_INDEX, LED_BUILTIN);
static char *messageOut;


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


void printErrorCodes() {
	ERROR_CODE **errors = input->getErrorCodes();

	errorIndex = 0;
	ERROR_CODE *code = errors[errorIndex];
	while(*code != ERROR_CODE::NONE) {
		Serial.println(*code);
		*code = ERROR_CODE::NONE;
		code = errors[++errorIndex];
	}
}


void loop() {
	processInputMethod();
	//peek messageToSend
	//Push peek to inputMessage - write inputMessage to LCD
	if(input->isMessageReady()) {
		Serial.print("message ready! : ");
		input->getMessageToSend(messageOut);
		Serial.println(messageOut);
		//sendMessage(inputMethod->getMessageToSend());
	}
	//messageIn = receiveMessage();
	//printMessage(); //write to LCD buffer, I guess

	printErrorCodes();
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

	messageOut = new char[MAX_MESSAGE_LENGTH];
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		messageOut[i] = '\0';
	}

	Serial.println("Running");
}
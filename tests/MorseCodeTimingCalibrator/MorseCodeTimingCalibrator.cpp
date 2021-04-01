#include "Arduino.h"
#include "HardwareSerial.h"


#define OPEN false
#define CLOSED true


static constexpr unsigned short SWITCH_PIN_INDEX = 9;
unsigned long lastDebounceTime = 0;
static constexpr unsigned short DEBOUNCE_DELAY = 25;

static bool switchState = OPEN;
static bool lastSwitchState = OPEN;
static bool pinValue = 0;

void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	pinMode(SWITCH_PIN_INDEX, INPUT);

	Serial.println("Ready");
}


void loop() {
	pinValue = digitalRead(SWITCH_PIN_INDEX) == HIGH ? CLOSED : OPEN;

	if(pinValue != lastSwitchState) {
		lastDebounceTime = millis();
	}

	if(millis() - lastDebounceTime > DEBOUNCE_DELAY) {
		if(pinValue == HIGH) {
			if(switchState != CLOSED) {
				Serial.println(millis());
				switchState = CLOSED;
			}
		} else {
			if(switchState != OPEN) {
				Serial.println(millis());
				switchState = OPEN;
			}
		}
	}

	lastSwitchState = pinValue;
}
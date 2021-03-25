#include "Arduino.h"
#include "HardwareSerial.h"


static constexpr unsigned short SWITCH_PIN_INDEX = 9;
static bool activeInputFlag = false;

void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	pinMode(SWITCH_PIN_INDEX, INPUT);

}


void loop() {
	while(true) {
		if(digitalRead(SWITCH_PIN_INDEX) == HIGH) {
			if(!activeInputFlag) {
				Serial.print("Down: ");
				Serial.println(millis());
				activeInputFlag = true;
			}
		} else {
			if(activeInputFlag) {
				Serial.print("Up: ");
				Serial.println(millis());
				activeInputFlag = false;
			}
		}
	}
}
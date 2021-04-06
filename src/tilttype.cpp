#include "include/tilttype.h"


TiltTypeInput::TiltTypeInput() {
	if(!IMU.begin()) {
		Serial.println("uh oh");
	}

	if(!IMU.accelerationAvailable() || !IMU.gyroscopeAvailable()) {
		Serial.println("Accelerometer and/or gyroscope not available!");
	}
}


TiltTypeInput::~TiltTypeInput() {

}


void TiltTypeInput::processInput(const unsigned long currentMilliseconds) {
	IMU.readAcceleration(accelerationX, accelerationY, accelerationZ);
	IMU.readGyroscope(gyroscopeX, gyroscopeY, gyroscopeZ);

	Serial.print("Acceleration: {");
	Serial.print(accelerationX);
	Serial.print(", ");
	Serial.print(accelerationY);
	Serial.print(", ");
	Serial.print(accelerationZ);
	Serial.println("}");

	Serial.print("Rotation: {");
	Serial.print(gyroscopeX);
	Serial.print(", ");
	Serial.print(gyroscopeY);
	Serial.print(", ");
	Serial.print(gyroscopeZ);
	Serial.println("}");

	delay(1000);
}
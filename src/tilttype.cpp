#include "include/tilttype.h"


TiltType::TiltType() {
	if (!IMU.accelerationAvailable() || !IMU.gyroscopeAvailable()) {
		Serial.println("Accelerometer and/or gyroscope not available!");
	}
}


TiltType::~TiltType() {

}


void TiltType::processInput(const unsigned long currentMilliseconds) {
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
}
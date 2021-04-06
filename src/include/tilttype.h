#ifndef TILTTYPE_H
#define TILTTYPE_H

#include "inputmethod.h"

#include <Arduino_LSM6DS3.h>

#include "Arduino.h"
#include "HardwareSerial.h"


class TiltTypeInput : public InputMethod {
public:
	TiltTypeInput();
	~TiltTypeInput();

	void setNetworkData(const char*) {}
	const char* getServerAddress() const {return nullptr;}
	const char* const* getRequestHeaders() const {return nullptr;}

	Pin **getPins() {return pins;}
	void processInput(const unsigned long);

	unsigned short getDebounceThreshold() {return 25;}
private:
	Pin *pins[1] = {&NULL_PIN};

	float accelerationX = 0.0f;
	float accelerationY = 0.0f;
	float accelerationZ = 0.0f;

	float gyroscopeX = 0.0f;
	float gyroscopeY = 0.0f;
	float gyroscopeZ = 0.0f;
};

#endif
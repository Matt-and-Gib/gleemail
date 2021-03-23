#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H

enum PIN_MODE : unsigned int {WRITE = 0, READ = 1};

struct Pin {
	Pin(const unsigned short i, const PIN_MODE m, const unsigned short v) {pinLocation = i; mode = m; value = v;}
	~Pin() {}

	short pinLocation;
	PIN_MODE mode;
	unsigned short value;

	bool operator== (const Pin &rhs) {
		return this->pinLocation == rhs.pinLocation;
	}

	bool operator!= (const Pin &rhs) {
		return this->pinLocation != rhs.pinLocation;
	}
};

static Pin *NULL_PIN = new Pin(-1, PIN_MODE::READ, 0);


class InputMethod {
public:
	virtual Pin **getPins() = 0;
	virtual Pin **getReadPins() = 0;
	virtual Pin **getWritePins() = 0;
	virtual void processInput(const unsigned long) = 0;
private:
};

#endif
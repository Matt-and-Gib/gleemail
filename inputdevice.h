#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

enum PIN_MODE : unsigned int {WRITE = 0, READ = 1};

struct Pin {
	Pin(const unsigned short i, const PIN_MODE m) {index = i; mode = m;}

	unsigned short index;
	PIN_MODE mode;

	bool operator== (const Pin &o) {
		return this->index == o.index;
	}

	bool operator!= (const Pin &o) {
		return this->index != o.index;
	}
};

static const Pin NULL_PIN = Pin(-1, PIN_MODE::READ);


class InputDevice {
public:
	virtual const Pin *getPins() = 0;
private:
};

#endif
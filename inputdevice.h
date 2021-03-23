#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

enum class PIN_MODE {WRITE = 0x0, READ = 0x1};

struct Pin {
	Pin(const unsigned short i, const PIN_MODE m) {index = i; mode = m;}

	unsigned short index;
	PIN_MODE mode;

	bool operator== (const Pin &o) {
		return this->index == o.index;
	}
};

static const Pin NULL_PIN = Pin(-1, PIN_MODE::READ);


class InputDevice {
public:
	virtual Pin *getPins() const;
private:
};

#endif
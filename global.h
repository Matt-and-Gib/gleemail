#ifndef GLOBAL_H
#define GLOBAL_H

static constexpr unsigned short BAUD_RATE = 9600;

static constexpr unsigned short MAX_MESSAGE_LENGTH = 32; //Maximum number of characters LCD display can print

enum LED_STATUS : unsigned short {OFF = 0, ON = 1};
enum PIN_MODE : unsigned int {WRITE = 1, READ = 0};

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

static Pin NULL_PIN = Pin(-1, PIN_MODE::READ, 0);

#endif
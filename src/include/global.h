#ifndef GLOBAL_H
#define GLOBAL_H

static constexpr unsigned short BAUD_RATE = 9600;

static constexpr char NETWORK_HEADER_USER_AGENT[] = "User-Agent: ArduinoWifiClient";
static constexpr char NETWORK_HEADER_ACCEPTED_RETURN_TYPE[] = "Accept: */*";
static constexpr char NETWORK_HEADER_CONNECTION_LIFETIME[] = "Connection: close";
static constexpr char HEADER_TERMINATION[] = "\r\n\r\n";
static constexpr unsigned short LENGTH_OF_HEADER_TERMINATION = sizeof(HEADER_TERMINATION)/sizeof(HEADER_TERMINATION[0]) - 1;

static constexpr unsigned short MAX_MESSAGE_LENGTH = 32; //Maximum number of characters LCD display can print

enum LED_STATUS : unsigned short {OFF = 0, ON = 1};
enum PIN_MODE : unsigned int {WRITE = 1, READ = 0};

enum ERROR_CODE: short {NONE = 0, MORSE_PHRASE_IMMINENT_OVERFLOW = 1, INPUT_MORSE_CHAR_NOTHING = 2};

struct Pin {
	Pin(const unsigned short i, const PIN_MODE m, const unsigned short v) {pinLocation = i; mode = m; value = v;}
	~Pin() {}

	short pinLocation;
	PIN_MODE mode;
	unsigned short value;

	bool operator==(const Pin &rhs) {
		return this->pinLocation == rhs.pinLocation;
	}

	bool operator!=(const Pin &rhs) {
		return this->pinLocation != rhs.pinLocation;
	}
};

static Pin NULL_PIN = Pin(-1, PIN_MODE::READ, 0);

#endif
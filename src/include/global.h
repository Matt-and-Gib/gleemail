#ifndef GLOBAL_H
#define GLOBAL_H

#include "debuglog.h"


static const constexpr char GLEEMAIL_VERSION[] = "alpha 0.02a";

static const constexpr bool OFFLINE_MODE = false;
static const constexpr bool VERBOSE_DEBUG_LOG = true;

static const constexpr unsigned short BAUD_RATE = 9600;

static const constexpr unsigned short MAX_IP_ADDRESS_LENGTH = 15;
static const constexpr unsigned short CONNECTION_PORT = 29453;
static const constexpr char NETWORK_HANDSHAKE_CHARACTER = '$';

static const constexpr char NETWORK_HEADER_USER_AGENT[] = "User-Agent: ArduinoWifiClient";
static const constexpr char NETWORK_HEADER_ACCEPTED_RETURN_TYPE[] = "Accept: */*";
static const constexpr char NETWORK_HEADER_CONNECTION_LIFETIME[] = "Connection: close";
static const constexpr char HEADER_TERMINATION[] = "\r\n\r\n";
static const constexpr unsigned short LENGTH_OF_HEADER_TERMINATION = sizeof(HEADER_TERMINATION)/sizeof(HEADER_TERMINATION[0]) - 1;

static const constexpr unsigned short MAX_MESSAGE_LENGTH = 16; //Maximum number of characters LCD display can print per line

static const constexpr char CANCEL_CHAR = (char)24;

enum LED_STATUS : unsigned short {OFF = 0, ON = 1};
enum PIN_MODE : unsigned int {WRITE = 1, READ = 0};

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
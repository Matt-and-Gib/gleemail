#ifndef GLOBAL_H
#define GLOBAL_H

#include "debuglog.h"


static const constexpr char GLEEMAIL_VERSION[] = "alpha 4";
static const constexpr bool OFFLINE_MODE = false;

static const constexpr unsigned short BAUD_RATE = 9600;

static const constexpr unsigned short STORAGE_SLAVE_SELECT_PIN = 10;

//NOTE: all paths are full caps, and file names must be in 8.3 format
static const constexpr char rootPath[9] = "GLEEMAIL";
static const constexpr char prefsPath[19] = "GLEEMAIL/PREFS.GMD";
static const constexpr char morseCodeCharPairsPath[18] = "GLEEMAIL/MCCP.GMD";

static const constexpr unsigned short FRAME_LATENCY_COUNT_ERROR_THRESHOLD = 3;
static const constexpr unsigned short MAX_FRAME_DURATION_MS = 42;
static const constexpr unsigned short MAX_NETWORKING_LOOP_DURATION_MS = 36;

static const constexpr unsigned short MAX_IP_ADDRESS_LENGTH = 15;
static const constexpr unsigned short CONNECTION_PORT = 29453;

static const constexpr char SERVER[] = "raw.githubusercontent.com";
static const constexpr char HOST[] = "Host: raw.githubusercontent.com";
static const constexpr char NETWORK_HEADER_USER_AGENT[] = "User-Agent: ArduinoWifiClient";
static const constexpr char NETWORK_HEADER_ACCEPTED_RETURN_TYPE[] = "Accept: */*";
static const constexpr char NETWORK_HEADER_CONNECTION_LIFETIME[] = "Connection: close";
static const constexpr char HEADER_TERMINATION[] = "\r\n\r\n";
static const constexpr unsigned short LENGTH_OF_HEADER_TERMINATION = sizeof(HEADER_TERMINATION)/sizeof(HEADER_TERMINATION[0]) - 1;

enum class MESSAGE_TYPE : unsigned short {ERROR = 0, HEARTBEAT = 1, CONFIRMATION = 2, CHAT = 3, HANDSHAKE = 4, NONE = 5};
static const constexpr MESSAGE_TYPE START_MESSAGE_TYPE = static_cast<MESSAGE_TYPE>(0);

static const constexpr unsigned short MAX_MESSAGE_LENGTH = 280; // WE'RE GOING OFF THE BEATEN TRAIL HERE!

static const constexpr unsigned short JSON_DOCUMENT_SIZE = 512; // Michael: remember why this is what it is!

/*
static const constexpr unsigned short MAX_MESSAGE_LENGTH = 140; // This value is dependent upon JSON_DOCUMENT_SIZE = 256.

static const constexpr unsigned short JSON_DOCUMENT_SIZE = 256; // This is the end of the line, the ultimate determiner of the size of the packets we cand send (bytes)!
*/

static const constexpr unsigned short JSON_DOCUMENT_FILTER_FOR_SIZE_BYTES = 32;

//static const constexpr unsigned short PREFS_DOCUMENT_SIZE = 384;

static const constexpr char CANCEL_CHAR = (char)24;

static char* copyString(const char* original, const unsigned short LEN) {
	char* duplicate = new char[LEN]; //on the heap
	for(short i = 0; i < LEN; i += 1) {
		duplicate[i] = original[i];
	}

	return duplicate;
}

static char* copyAndTerminateString(const char* original, const unsigned short LEN) {
	char* duplicate = new char[LEN + 1]; //on the heap
	for(short i = 0; i < LEN; i += 1) {
		duplicate[i] = original[i];
	}
	duplicate[LEN] = '\0';
	
	return duplicate;
}


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
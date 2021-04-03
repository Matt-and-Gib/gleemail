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


enum ERROR_CODE: short {
	NONE = 0,
	DEBUG_DEBUG_LOG = 1,

	//InputMethod range: 2 - 3
	MORSE_PHRASE_IMMINENT_OVERFLOW = 2,
	INPUT_MORSE_CHAR_NOTHING = 3,

	//Network range: 4 - 6
	NETWORK_PASSED_INVALID_PARAMETER = 4,
	NETWORK_WIFI_CONNECTION_FAILED_RETRY_OCCURED = 5,
	NETWORK_DOWNLOAD_IMPOSSIBLE_NOT_CONNECTED = 6
};

//Remember: DebugLog is a singleton! DO NOT waste memory.
class DebugLog {
public:
	static DebugLog& getLog() {
		static DebugLog log;
		return log;
	}

	void logError(ERROR_CODE e) {
		if(errorCodesFirstOpenIndex < MAX_ERROR_CODES) {
			errorCodes[errorCodesFirstOpenIndex++] = e;
		}
	}


	ERROR_CODE getError() {
		if(errorCodesFirstOpenIndex > 0) {
			return errorCodes[--errorCodesFirstOpenIndex];
		} else {
			return ERROR_CODE::NONE;
		}
	}
private:
	DebugLog() {
		errorCodes = new ERROR_CODE[MAX_ERROR_CODES];
		for(int i = 0; i < MAX_ERROR_CODES; i += 1) {
			errorCodes[i] = ERROR_CODE::NONE;
		}
	}
	DebugLog(DebugLog const&) = delete;
	void operator=(DebugLog const&) = delete;

	static constexpr unsigned short MAX_ERROR_CODES = 16;
	ERROR_CODE* errorCodes;
	unsigned short errorCodesFirstOpenIndex = 0;
};


#endif
#ifndef GLOBAL_H
#define GLOBAL_H

static const constexpr char GLEEMAIL_VERSION[] = "alpha 0.02a";

static const constexpr bool OFFLINE_MODE = true;

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


static const constexpr bool VERBOSE_DEBUG_LOG = true;

static const constexpr unsigned short DEBUG_LOG_INPUT_METHOD_OFFSET = 10;
static const constexpr unsigned short DEBUG_LOG_NETWORK_OFFSET = 50;
static const constexpr unsigned short DEBUG_LOG_JSON_OFFSET = 90;
enum ERROR_CODE: short {
	//Meta range: 0 - 9
	NONE = 0,
	DEBUG_DEBUG_LOG = 1,
	UNKNOWN_SETUP_STATE = 2,

	//InputMethod range: 10 - 49
	MORSE_PHRASE_IMMINENT_OVERFLOW = DEBUG_LOG_INPUT_METHOD_OFFSET + 0,
	//INPUT_MORSE_CHAR_NOTHING = DEBUG_LOG_INPUT_METHOD_OFFSET + 1, //unused
	MORSE_CODE_LOOKUP_FAILED = DEBUG_LOG_INPUT_METHOD_OFFSET + 2, //VERBOSE DEBUG ONLY
	MORSE_MESSAGE_TO_SEND_REACHED_MAX_MESSAGE_LENGTH = DEBUG_LOG_INPUT_METHOD_OFFSET + 3,
	MORSE_MESSAGE_TO_SEND_EXCEEDED_MAX_MESSAGE_LENGTH = DEBUG_LOG_INPUT_METHOD_OFFSET + 4,

	//Network range: 50 - 89
	NETWORK_CONNECTION_FAILED = DEBUG_LOG_NETWORK_OFFSET + 0,
	NETWORK_PASSED_INVALID_PARAMETER = DEBUG_LOG_NETWORK_OFFSET + 1,
	NETWORK_WIFI_CONNECTION_FAILED_RETRY_OCCURED = DEBUG_LOG_NETWORK_OFFSET + 2,
	NETWORK_DOWNLOAD_IMPOSSIBLE_NOT_CONNECTED = DEBUG_LOG_NETWORK_OFFSET + 3,
	NETWORK_SECURE_CONNECTION_TO_SERVER_FAILED = DEBUG_LOG_NETWORK_OFFSET + 4,
	NETWORK_REQUEST_TO_SERVER_HEADER_INVALID = DEBUG_LOG_NETWORK_OFFSET + 5,
	NETWORK_HEADER_TERMINATION_OMITTED = DEBUG_LOG_NETWORK_OFFSET + 6,
	NETWORK_DATA_BUFFER_OVERFLOW = DEBUG_LOG_NETWORK_OFFSET + 7,
	NETWORK_DATA_BUFFER_UNDERUTILIZED = DEBUG_LOG_NETWORK_OFFSET + 8,
	NETWORK_DATA_SSID_POSSIBLY_TRUNCATED = DEBUG_LOG_NETWORK_OFFSET + 9,
	NETWORK_DATA_PASSWORD_POSSIBLY_TRUNCATED = DEBUG_LOG_NETWORK_OFFSET + 10,
	NETWORK_INVALID_HANDSHAKE_MESSAGE = DEBUG_LOG_NETWORK_OFFSET + 11,
	NETWORK_UNEXPECTED_HANDSHAKE_IP = DEBUG_LOG_NETWORK_OFFSET + 12,
	NETWORK_INVALID_PEER_IP_ADDRESS = DEBUG_LOG_NETWORK_OFFSET + 13,
	NETWORK_UNKNOWN_STATUS = DEBUG_LOG_NETWORK_OFFSET + 14,

	//JSON range: 90 - 129
	JSON_NULLPTR_PAYLOAD = DEBUG_LOG_JSON_OFFSET + 0,
	JSON_DESERIALIZATION_ERROR = DEBUG_LOG_JSON_OFFSET + 1
};


//Remember: DebugLog is a singleton! DO NOT waste memory.
class DebugLog {
public:
	static DebugLog& getLog() {
		static DebugLog log;
		return log;
	}


	void logError(ERROR_CODE e) {
		log(e, true);
	}

	void logWarning(ERROR_CODE e) {
		log(e, false);
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

	void log(ERROR_CODE e, bool critical) {
		if(critical || (!critical && VERBOSE_DEBUG_LOG)) {
			if(errorCodesFirstOpenIndex < MAX_ERROR_CODES) {
				errorCodes[errorCodesFirstOpenIndex++] = e;
			}
		}
	}

	static constexpr unsigned short MAX_ERROR_CODES = 16;
	ERROR_CODE* errorCodes;
	unsigned short errorCodesFirstOpenIndex = 0;
};


#endif
#ifndef DEBUGLOG_H
#define DEBUGLOG_H


static const constexpr bool VERBOSE_DEBUG_LOG = true;


static const constexpr unsigned short DEBUG_LOG_INPUT_METHOD_OFFSET = 10;
static const constexpr unsigned short DEBUG_LOG_NETWORK_OFFSET = 50;
static const constexpr unsigned short DEBUG_LOG_JSON_OFFSET = 90;
static const constexpr unsigned short DEBUG_LOG_MESSAGE_ERROR_OFFSET = 130;
static const constexpr unsigned short DEBUG_LOG_INTERNET_ACCESS_OFFSET = 170;
static const constexpr unsigned short DEBUG_LOG_WEB_ACCESS_OFFSET = 210;
enum ERROR_CODE: short {
	//Meta range: 0 - 9
	NONE = 0,
	DEBUG_DEBUG_LOG = 1,
	UNKNOWN_SETUP_STATE = 2,
	CONTINUOUS_FRAME_LATENCY = 3,

	//InputMethod range: 10 - 49
	MORSE_PHRASE_IMMINENT_OVERFLOW = DEBUG_LOG_INPUT_METHOD_OFFSET + 0,
	//INPUT_MORSE_CHAR_NOTHING = DEBUG_LOG_INPUT_METHOD_OFFSET + 1, //unused
	MORSE_CODE_LOOKUP_FAILED = DEBUG_LOG_INPUT_METHOD_OFFSET + 2,
	MORSE_MESSAGE_TO_SEND_REACHED_MAX_MESSAGE_LENGTH = DEBUG_LOG_INPUT_METHOD_OFFSET + 3,
	MORSE_MESSAGE_TO_SEND_EXCEEDED_MAX_MESSAGE_LENGTH = DEBUG_LOG_INPUT_METHOD_OFFSET + 4,
	INPUT_METHOD_MESSAGE_CONTAINS_PRECEDING_WHITESPACE = DEBUG_LOG_INPUT_METHOD_OFFSET + 5,
	INPUT_METHOD_MESSAGE_CONTAINS_TRAILING_WHITESPACE = DEBUG_LOG_INPUT_METHOD_OFFSET + 6,
	INPUT_METHOD_COMMIT_EMPTY_MESSAGE = DEBUG_LOG_INPUT_METHOD_OFFSET + 7,

	//Network range: 50 - 89
	/*
	NETWORK_INVALID_HANDSHAKE_MESSAGE = DEBUG_LOG_NETWORK_OFFSET + 11,
	NETWORK_UNEXPECTED_HANDSHAKE_IP = DEBUG_LOG_NETWORK_OFFSET + 12,
	NETWORK_INVALID_PEER_IP_ADDRESS = DEBUG_LOG_NETWORK_OFFSET + 13,
	NETWORK_WRITE_FAILED = DEBUG_LOG_NETWORK_OFFSET + 15,
	NETWORK_PEER_MESSAGE_READ_FAILED = DEBUG_LOG_NETWORK_OFFSET + 16,
	*/
	//^Possibly unused network errors above

	NETWORK_UNKNOWN_MESSAGE_SENDER = DEBUG_LOG_NETWORK_OFFSET + 1,
	NETWORK_TOO_MANY_MESSAGES_RECEIVED = DEBUG_LOG_NETWORK_OFFSET + 2,
	NETWORK_HEARTBEAT_FLATLINE = DEBUG_LOG_NETWORK_OFFSET + 3,
	NETWORK_OUTGOING_TOKEN_TIMESTAMP_ELAPSED = DEBUG_LOG_NETWORK_OFFSET + 4,
	NETWORK_TIME_SENSITIVE_PROCESS_EXCEEDED_ALLOCATED_TIME_SIGNIFICANT = DEBUG_LOG_NETWORK_OFFSET + 5,
	NETWORK_TIME_SENSITIVE_PROCESS_EXCEEDED_ALLOCATED_TIME_INSIGNFICANT = DEBUG_LOG_NETWORK_OFFSET + 6,
	NETWORK_UNKNOWN_INCOMING_MESSAGE_TYPE = DEBUG_LOG_NETWORK_OFFSET + 7,
	NETWORK_CONFIRMATION_NO_MATCH_FOUND = DEBUG_LOG_NETWORK_OFFSET + 8,
	NETWORK_DUPLICATE_HANDSHAKE = DEBUG_LOG_NETWORK_OFFSET + 9,

	//JSON range: 90 - 129
	JSON_NULLPTR_PAYLOAD = DEBUG_LOG_JSON_OFFSET + 0,
	JSON_DESERIALIZATION_ERROR = DEBUG_LOG_JSON_OFFSET + 1,
	JSON_MESSAGE_DESERIALIZATION_ERROR = DEBUG_LOG_JSON_OFFSET + 2,

	//MessageError range: 130 - 169
	MESSAGE_ERROR_NONE = DEBUG_LOG_MESSAGE_ERROR_OFFSET + 0,

	//InternetAccess range: 170 - 209
	INTERNET_ACCESS_PASSED_INVALID_PARAMETER = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 0,
	INTERNET_ACCESS_CONNECTION_FAILED = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 1,
	INTERNET_ACCESS_WIFI_CONNECTION_FAILED_RETRY_OCCURRED = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 2,
	INTERNET_ACCESS_UNKNOWN_STATUS = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 3,
	INTERNET_ACCESS_SSID_POSSIBLY_TRUNCATED = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 4,
	INTERNET_ACCESS_PASSWORD_POSSIBLY_TRUNCATED = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 5,

	//WebAccess range: 210 - 250
	WEB_ACCESS_HEADER_TERMINATION_OMITTED = DEBUG_LOG_WEB_ACCESS_OFFSET + 0,
	WEB_ACCESS_DOWNLOAD_IMPOSSIBLE_NOT_CONNECTED = DEBUG_LOG_WEB_ACCESS_OFFSET + 1,
	WEB_ACCESS_SECURE_CONNECTION_TO_SERVER_FAILED = DEBUG_LOG_WEB_ACCESS_OFFSET + 2,
	WEB_ACCESS_REQUEST_TO_SERVER_HEADER_INVALID = DEBUG_LOG_WEB_ACCESS_OFFSET + 3,
	WEB_ACCESS_DATA_BUFFER_OVERFLOW = DEBUG_LOG_WEB_ACCESS_OFFSET + 4,
	WEB_ACCESS_DATA_BUFFER_UNDERUTILIZED = DEBUG_LOG_WEB_ACCESS_OFFSET + 5
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


	ERROR_CODE getNextError() {
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
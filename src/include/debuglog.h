#ifndef DEBUGLOG_H
#define DEBUGLOG_H


namespace GLEEMAIL_DEBUG {
	const constexpr unsigned short DEBUG_LOG_INPUT_METHOD_OFFSET = 10;
	const constexpr unsigned short DEBUG_LOG_NETWORK_OFFSET = 50;
	const constexpr unsigned short DEBUG_LOG_JSON_OFFSET = 90;
	const constexpr unsigned short DEBUG_LOG_MESSAGE_ERROR_OFFSET = 130;
	const constexpr unsigned short DEBUG_LOG_INTERNET_ACCESS_OFFSET = 170;
	const constexpr unsigned short DEBUG_LOG_WEB_ACCESS_OFFSET = 210;
	const constexpr unsigned short DEBUG_LOG_STORAGE_OFFSET = 250;
	const constexpr unsigned short DEBUG_LOG_DISPLAY_OFFSET = 290;
	const constexpr unsigned short DEBUG_LOG_PREFERENCES_OFFSET = 330;
	const constexpr unsigned short DEBUG_LOG_STARTUP_CODE_OFFSET = 370;
	enum ERROR_CODE: short {
		//Meta range: 0 - 9
		NONE = 0,
		DEBUG_DEBUG_LOG = 1,
		UNKNOWN_SETUP_STATE = 2,
		CONTINUOUS_FRAME_LATENCY = 3,
		DEBUG_OVERFLOW_ERRORS_LOST = 4,

		//InputMethod range: 10 - 49
		//MORSE_PHRASE_IMMINENT_OVERFLOW = DEBUG_LOG_INPUT_METHOD_OFFSET + 0, //unused
		MORSE_PUSHED_CHAR_NOTHING = DEBUG_LOG_INPUT_METHOD_OFFSET + 1,
		MORSE_CODE_LOOKUP_FAILED = DEBUG_LOG_INPUT_METHOD_OFFSET + 2, //WARNING
		MORSE_MESSAGE_TO_SEND_REACHED_CHAT_COMPLETE_THRESHOLD = DEBUG_LOG_INPUT_METHOD_OFFSET + 3, //WARNING
		MORSE_MESSAGE_TO_SEND_EXCEEDED_CHAT_COMPLETE_THRESHOLD = DEBUG_LOG_INPUT_METHOD_OFFSET + 4,
		//INPUT_METHOD_MESSAGE_CONTAINS_PRECEDING_WHITESPACE = DEBUG_LOG_INPUT_METHOD_OFFSET + 5, //WARNING //unused
		//INPUT_METHOD_MESSAGE_CONTAINED_TRAILING_WHITESPACE = DEBUG_LOG_INPUT_METHOD_OFFSET + 6, //WARNING //unused
		INPUT_METHOD_COMMIT_EMPTY_MESSAGE = DEBUG_LOG_INPUT_METHOD_OFFSET + 7,
		INPUT_METHOD_VERSION_NUMBER_OVERFLOW = DEBUG_LOG_INPUT_METHOD_OFFSET + 8,
		INPUT_METHOD_MORSE_CODE_CHAR_PAIRS_VERSION_MISMATCH = DEBUG_LOG_INPUT_METHOD_OFFSET + 9,
		INPUT_METHOD_MESSAGE_ONLY_WHITESPACE = DEBUG_LOG_INPUT_METHOD_OFFSET + 10,
		INPUT_METHOD_DATA_VERSION_DOWNLOAD_FAILED = DEBUG_LOG_INPUT_METHOD_OFFSET + 11, //WARNING
		INPUT_METHOD_DATA_DOWNLOAD_FAILED = DEBUG_LOG_INPUT_METHOD_OFFSET + 12, //WARNING
		//MORSE_INSERTING_INTO_TREE_FAILED = DEBUG_LOG_INPUT_METHOD_OFFSET + 13,
		MORSE_NULLPTR_PAYLOAD = DEBUG_LOG_INPUT_METHOD_OFFSET + 14,
		MORSE_FILTERED_JSON_PAYLOAD_SIZE_ZERO = DEBUG_LOG_INPUT_METHOD_OFFSET + 15,

		//Network range: 50 - 89
		NETWORK_UNKNOWN_MESSAGE_SENDER = DEBUG_LOG_NETWORK_OFFSET + 1,
		NETWORK_TOO_MANY_MESSAGES_RECEIVED = DEBUG_LOG_NETWORK_OFFSET + 2,
		NETWORK_HEARTBEAT_FLATLINE = DEBUG_LOG_NETWORK_OFFSET + 3,
		NETWORK_OUTGOING_TOKEN_TIMESTAMP_ELAPSED = DEBUG_LOG_NETWORK_OFFSET + 4,
		//NETWORK_TIME_SENSITIVE_PROCESS_EXCEEDED_ALLOCATED_TIME_SIGNIFICANT = DEBUG_LOG_NETWORK_OFFSET + 5, //unused
		//NETWORK_TIME_SENSITIVE_PROCESS_EXCEEDED_ALLOCATED_TIME_INSIGNIFICANT = DEBUG_LOG_NETWORK_OFFSET + 6, //unused
		NETWORK_UNKNOWN_INCOMING_MESSAGE_TYPE = DEBUG_LOG_NETWORK_OFFSET + 7,
		NETWORK_CONFIRMATION_NO_MATCH_FOUND = DEBUG_LOG_NETWORK_OFFSET + 8, //WARNING
		NETWORK_DUPLICATE_HANDSHAKE = DEBUG_LOG_NETWORK_OFFSET + 9, //WARNING
		NETWORK_UNEXPECTED_HANDSHAKE_FROM_CONNECTED_IP = DEBUG_LOG_NETWORK_OFFSET + 10, //WARNING
		NETWORK_HEARTBEAT_STILLBORN = DEBUG_LOG_NETWORK_OFFSET + 11,
		NETWORK_CONNECTION_ATTEMPT_WHILE_CONNECTED = DEBUG_LOG_NETWORK_OFFSET + 12, //WARNING
		NETWORK_AUTHENTICATION_FAILED = DEBUG_LOG_NETWORK_OFFSET + 13,
		NETWORK_GET_MESSAGES_EXCEEDED_ALLOCATED_TIME_SIGNIFICANT = DEBUG_LOG_NETWORK_OFFSET + 14,
		NETWORK_GET_MESSAGES_EXCEEDED_ALLOCATED_TIME_INSIGNIFICANT = DEBUG_LOG_NETWORK_OFFSET + 15, //WARNING
		NETWORK_MESSAGES_IN_EXCEEDED_ALLOCATED_TIME_SIGNIFICANT = DEBUG_LOG_NETWORK_OFFSET + 16,
		NETWORK_MESSAGES_IN_EXCEEDED_ALLOCATED_TIME_INSIGNIFICANT = DEBUG_LOG_NETWORK_OFFSET + 17, //WARNING
		NETWORK_MESSAGES_OUT_EXCEEDED_ALLOCATED_TIME_SIGNIFICANT = DEBUG_LOG_NETWORK_OFFSET + 18,
		NETWORK_MESSAGES_OUT_EXCEEDED_ALLOCATED_TIME_INSIGNIFICANT = DEBUG_LOG_NETWORK_OFFSET + 19, //WARNING
		NETWORK_RECEIVED_UNAUTHENTIC_MESSAGE = DEBUG_LOG_NETWORK_OFFSET + 20,

		//JSON range: 90 - 129
		//JSON_NULLPTR_PAYLOAD = DEBUG_LOG_JSON_OFFSET + 0,
		JSON_PREFS_DESERIALIZATION_ERROR = DEBUG_LOG_JSON_OFFSET + 1,
		JSON_MESSAGE_DESERIALIZATION_ERROR = DEBUG_LOG_JSON_OFFSET + 2,
		JSON_INPUT_DATA_DESERIALIZATION_ERROR = DEBUG_LOG_JSON_OFFSET + 3,
		JSON_MORSECODE_NETWORK_DATA_DESERIALIZATION_ERROR = DEBUG_LOG_JSON_OFFSET + 4,
		JSON_PREFS_FILTERED_DESERIALIZATION_ERROR = DEBUG_LOG_JSON_OFFSET + 5,
		JSON_MORSECODE_FILTER_DESERIALIZATION_ERROR = DEBUG_LOG_JSON_OFFSET + 6,

		//MessageError range: 130 - 169
		MESSAGE_ERROR_NONE = DEBUG_LOG_MESSAGE_ERROR_OFFSET + 0, //WARNING

		//InternetAccess range: 170 - 209
		INTERNET_ACCESS_PASSED_INVALID_PARAMETER = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 0,
		INTERNET_ACCESS_CONNECTION_FAILED = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 1, //WARNING
		INTERNET_ACCESS_WIFI_CONNECTION_FAILED_RETRY_OCCURRED = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 2,
		INTERNET_ACCESS_UNKNOWN_STATUS = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 3,
		INTERNET_ACCESS_SSID_POSSIBLY_TRUNCATED = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 4,
		INTERNET_ACCESS_PASSWORD_POSSIBLY_TRUNCATED = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 5,
		INTERNET_ACCESS_DISCONNECTED_DURING_CONNECTION_ATTEMPT = DEBUG_LOG_INTERNET_ACCESS_OFFSET + 6,

		//WebAccess range: 210 - 249
		WEB_ACCESS_REQUEST_HEADER_TERMINATION_OMITTED = DEBUG_LOG_WEB_ACCESS_OFFSET + 0,
		WEB_ACCESS_DOWNLOAD_IMPOSSIBLE_NOT_CONNECTED = DEBUG_LOG_WEB_ACCESS_OFFSET + 1,
		WEB_ACCESS_SECURE_CONNECTION_TO_SERVER_FAILED = DEBUG_LOG_WEB_ACCESS_OFFSET + 2,
		WEB_ACCESS_REQUEST_TO_SERVER_HEADER_INVALID = DEBUG_LOG_WEB_ACCESS_OFFSET + 3,
		WEB_ACCESS_DATA_BUFFER_OVERFLOW = DEBUG_LOG_WEB_ACCESS_OFFSET + 4,
		WEB_ACCESS_DATA_BUFFER_UNDERUTILIZED = DEBUG_LOG_WEB_ACCESS_OFFSET + 5, //WARNING
		WEB_ACCESS_DATA_BUFFER_EMPTY = DEBUG_LOG_WEB_ACCESS_OFFSET + 6,
		WEB_ACCESS_RESPONSE_HEADER_TERMINATION_NOT_FOUND = DEBUG_LOG_WEB_ACCESS_OFFSET + 7,

		//Storage range: 250 - 289
		STORAGE_NOT_DETECTED = DEBUG_LOG_STORAGE_OFFSET + 0, //WARNING
		STORAGE_ALREADY_INITIALIZED = DEBUG_LOG_STORAGE_OFFSET + 2, //WARNING
		STORAGE_UNINITIALIZED_WRITE = DEBUG_LOG_STORAGE_OFFSET + 3,
		STORAGE_ROOT_FS_ACCESS_FAILED = DEBUG_LOG_STORAGE_OFFSET + 4,
		STORAGE_CREATE_GLEEMAIL_ROOT_FAILED = DEBUG_LOG_STORAGE_OFFSET + 5,
		STORAGE_WRITE_OPEN_FAILED = DEBUG_LOG_STORAGE_OFFSET + 6,
		STORAGE_UNINITIALIZED_READ = DEBUG_LOG_STORAGE_OFFSET + 7,
		STORAGE_READ_OPEN_FAILED = DEBUG_LOG_STORAGE_OFFSET + 8,
		STORAGE_UNCONFIRMED_ERASE = DEBUG_LOG_STORAGE_OFFSET + 9,
		STORAGE_UNINITIALIZED_ERASE_ALL = DEBUG_LOG_STORAGE_OFFSET + 10, //WARNING
		STORAGE_OPEN_GLEEMAIL_ROOT_FOR_ERASE_FAILED = DEBUG_LOG_STORAGE_OFFSET + 11, //WARNING
		STORAGE_GLEEMAIL_ROOT_IS_FILE = DEBUG_LOG_STORAGE_OFFSET + 12,
		STORAGE_UNINITIALIZED_ERASE = DEBUG_LOG_STORAGE_OFFSET + 13,
		STORAGE_OBJECT_UNSUCCESSFULLY_ALLOCATED = DEBUG_LOG_STORAGE_OFFSET + 14,

		//Display range: 290 - 329
		DISPLAY_RAM_INTEGRITY_THREATENED = DEBUG_LOG_DISPLAY_OFFSET + 0,

		//Preferences range: 330 - 369
		PREFERENCES_LOAD_FAILED = DEBUG_LOG_PREFERENCES_OFFSET + 0, //WARNING
		PREFERENCES_FILE_SIZE_MISMATCH = DEBUG_LOG_PREFERENCES_OFFSET + 1, //WARNING
		PREFERENCES_FILE_VERSION_MISMATCH = DEBUG_LOG_STORAGE_OFFSET + 2, //WARNING

		//Startup Codes range: 370 - 409
		STARTUP_CODE_UNEXPECTED_SERIAL_INPUT = DEBUG_LOG_STARTUP_CODE_OFFSET + 0, //WARNING
		STARTUP_CODE_MALFORMED_PROMPT_TRIGGER = DEBUG_LOG_STARTUP_CODE_OFFSET + 1,
		STARTUP_CODE_TOO_MANY_CODES_PROVIDED = DEBUG_LOG_STARTUP_CODE_OFFSET + 2,
		STARTUP_CODE_DUPLICATE_CODES_NOT_ALLOWED = DEBUG_LOG_STARTUP_CODE_OFFSET + 3 //WARNING
	};


	class DebugLog final {
	private:
		const bool VERBOSE_DEBUG_LOG = true;
		static constexpr unsigned short MAX_ERROR_CODES = 16;
		ERROR_CODE* errorCodes;
		unsigned short errorCodesFirstOpenIndex = 0;
		bool overflowErrorsLost = false;

		explicit DebugLog() {
			errorCodes = new ERROR_CODE[MAX_ERROR_CODES];
			for(int i = 0; i < MAX_ERROR_CODES; i += 1) {
				errorCodes[i] = ERROR_CODE::NONE;
			}
		}


		void log(ERROR_CODE e, bool critical) {
			if(critical || (!critical && VERBOSE_DEBUG_LOG)) {
				if(errorCodesFirstOpenIndex < MAX_ERROR_CODES) {
					errorCodes[++errorCodesFirstOpenIndex] = e;
				} else {
					overflowErrorsLost = true;
				}
			}
		}
	public:
		explicit DebugLog(DebugLog const&) = delete;
		void operator=(DebugLog const&) = delete;
		~DebugLog() = default;

		static DebugLog& getLog() {
			static DebugLog log;
			return log;
		}


		bool verboseMode() const {return VERBOSE_DEBUG_LOG;}

		void logError(ERROR_CODE e) {log(e, true);}
		void logWarning(ERROR_CODE e) {log(e, false);}

		ERROR_CODE getNextError() {
			if(errorCodesFirstOpenIndex > 0) {
				return errorCodes[--errorCodesFirstOpenIndex];
			} else if(overflowErrorsLost == true) {
				overflowErrorsLost = false;
				return ERROR_CODE::DEBUG_OVERFLOW_ERRORS_LOST;
			} else {
				return ERROR_CODE::NONE;
			}
		}


		unsigned short getPendingErrors() const {
			return errorCodesFirstOpenIndex;
		}
	};
}


#endif
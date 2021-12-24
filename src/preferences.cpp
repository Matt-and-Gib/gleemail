#include "include/preferences.h"

#include "include/global.h"

#include <ArduinoJson.h>


Preferences& Preferences::getPrefs() {
	static Preferences prefs;
	return prefs;
}


void Preferences::setWiFiSSID(char* s) {
	if(!s || s[0] == '\0') {
		wifiSSID = nullptr;
		delete[] s;	//REMEMBER: Provided ssid pointer will be deleted if invalid!
	} else {
		wifiSSID = s;
	}
}


void Preferences::setWiFiPassword(char* p) {
	if(!p || p[0] == '\0') {
		wifiPassword = nullptr;
		delete[] p; //REMEMBER: Provided password pointer will be deleted if invalid!
	} else {
		wifiPassword = p;
	}
}


const char* Preferences::serializePrefs() const {
		DynamicJsonDocument doc = DynamicJsonDocument(CALCULATED_PREFS_SIZE);

		doc["Size"] = CALCULATED_PREFS_SIZE;
		doc["Preferences Version"] = PREFERENCES_VERSION;
		doc["Morse Code Char Pairs Version"] = getMorseCodeCharPairsVersion();

		doc["WiFiSSID"] = getWiFiSSID();
		doc["WiFiPassword"] = getWiFiPassword();

		const unsigned short outputSize = measureJson(doc);
		char output[outputSize];

		serializeJson(doc, output, outputSize);
		return copyAndTerminateString(output, outputSize);
	}


bool Preferences::deserializePrefs(const char* input, const unsigned short length) {
		StaticJsonDocument<JSON_DOCUMENT_FILTER_FOR_SIZE_BYTES> filter;
		filter["Size"] = true;

		StaticJsonDocument<JSON_DOCUMENT_FILTER_FOR_SIZE_BYTES> sizeDoc;
		DeserializationError error = deserializeJson(sizeDoc, input, DeserializationOption::Filter(filter));
		if(error) {
			GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::JSON_PREFS_FILTERED_DESERIALIZATION_ERROR);
			//Serial.print(F("filter deserialization error: "));
			//Serial.println(error.c_str());
			return false;
		}

		const unsigned short size = sizeDoc["Size"];
		if(size != CALCULATED_PREFS_SIZE) {
			GLEEMAIL_DEBUG::DebugLog::getLog().logWarning(GLEEMAIL_DEBUG::ERROR_CODE::PREFERENCES_FILE_SIZE_MISMATCH);
			//return false;
		}

		DynamicJsonDocument doc(size);
		error = deserializeJson(doc, input, length);

		if(error) {
			GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::JSON_PREFS_DESERIALIZATION_ERROR);
			return false;
		}

		const unsigned short preferencesFileVersion = doc["Preferences Version"];
		if(preferencesFileVersion != PREFERENCES_VERSION) {
			GLEEMAIL_DEBUG::DebugLog::getLog().logWarning(GLEEMAIL_DEBUG::ERROR_CODE::PREFERENCES_FILE_VERSION_MISMATCH);
		}

		morseCodeCharPairsVersion = doc["Morse Code Char Pairs Version"];

		const char* tempSSID = doc["WiFiSSID"];
		setWiFiSSID(copyAndTerminateString(tempSSID, strlen(tempSSID)));

		const char* tempPassword = doc["WiFiPassword"];
		setWiFiPassword(copyAndTerminateString(tempPassword, strlen(tempPassword)));

		return true;
	}
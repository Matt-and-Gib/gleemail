#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <ArduinoJson.h>

#include "global.h"


//Remember: Preferences is a singleton! DO NOT waste memory.
class Preferences {
private:
	Preferences() {
		wifiSSID = nullptr;
		wifiPassword = nullptr;
	}
	Preferences(Preferences const&) = delete;
	void operator=(Preferences const&) = delete;

	const static unsigned short PREFERENCES_VERSION = 1;
	const static unsigned short CALCULATED_PREFS_SIZE = 384; //This value represents the size of the current (most up-to-date) version of the preferences file, possibly different than what is on the SD

	unsigned short morseCodeCharPairsVersion = 3;
	char* wifiSSID;
	char* wifiPassword;
public:
	static Preferences& getPrefs() {
		static Preferences prefs;
		return prefs;
	}

	const char* getWiFiSSID() const {return wifiSSID;}
	void setWiFiSSID(char* s) {wifiSSID = s;}

	const char* getWiFiPassword() const {return wifiPassword;}
	void setWiFiPassword(char* p) {wifiPassword = p;}

	const char* serializePrefs() const {
		DynamicJsonDocument doc(CALCULATED_PREFS_SIZE);

		doc["Size"] = CALCULATED_PREFS_SIZE;
		doc["Preferences Version"] = PREFERENCES_VERSION;
		doc["Morse Code Char Pairs Version"] = morseCodeCharPairsVersion;
		if(wifiSSID) {
			doc["WiFiSSID"] = wifiSSID;
		}

		if(wifiPassword) {
			doc["WiFiPassword"] = wifiPassword;;
		}

		const unsigned short outputSize = measureJson(doc);
		char output[outputSize];

		serializeJson(doc, output, outputSize);
		return copyAndTerminateString(output, outputSize);
	}

	bool deserializePrefs(const char* input, const unsigned short length) {
		StaticJsonDocument<JSON_DOCUMENT_FILTER_FOR_SIZE_BYTES> filter;
		filter["Size"] = true;

		StaticJsonDocument<JSON_DOCUMENT_FILTER_FOR_SIZE_BYTES> sizeDoc;
		deserializeJson(sizeDoc, input, DeserializationOption::Filter(filter));
		const unsigned short size = sizeDoc["Size"];

		if(size != CALCULATED_PREFS_SIZE) {
			//DebugLog::getLog().logWarning(PREFERENCES_SIZE_MISMATCH);
		}




		DynamicJsonDocument doc(size);
		DeserializationError error = deserializeJson(doc, input, length);

		if(error) {
			//Serial.print(F("deserializeJson() failed: "));
			//Serial.println(error.f_str());
			DebugLog::getLog().logError(ERROR_CODE::JSON_PREFS_DESERIALIZATION_ERROR);
			return false;
		}

		const unsigned short preferencesFileVersion = doc["Preferences Version"];
		if(preferencesFileVersion != PREFERENCES_VERSION) {
			//DebugLog::getLog().logWarning(PREFERENCES_FILE_VERSION_MISMATCH);
		}


		morseCodeCharPairsVersion = doc["Morse Code Char Pairs Version"];

		const char* tempSSID = doc["WiFiSSID"];
		wifiSSID = copyAndTerminateString(tempSSID, strlen(tempSSID));

		const char* tempPassword = doc["WiFiPassword"];
		wifiPassword = copyAndTerminateString(tempPassword, strlen(tempPassword));
	}
};

#endif
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

	unsigned short morseCodeCharPairsVersion = 0;
	char* wifiSSID = nullptr;
	char* wifiPassword = nullptr;
public:
	static Preferences& getPrefs() {
		static Preferences prefs; //wrong for singleton pattern!
		return prefs;
	}

	const char* getWiFiSSID() const {return wifiSSID;}
	void setWiFiSSID(char* s) {
		if(!s || s[0] == '\0') {
			wifiSSID = nullptr;
			delete[] s;	//REMEMBER: Provided ssid pointer will be deleted if invalid!
		} else {
			wifiSSID = s;
		}
	}

	const char* getWiFiPassword() const {return wifiPassword;}
	void setWiFiPassword(char* p) {
		if(!p || p[0] == '\0') {
			wifiPassword = nullptr;
			delete[] p; //REMEMBER: Provided password pointer will be deleted if invalid!
		} else {
			wifiPassword = p;
		}
	}

	unsigned short getMorseCodeCharPairsVersion() {return morseCodeCharPairsVersion;}
	void setMorseCodeCharPairsVersion(const unsigned short v) {morseCodeCharPairsVersion = v;}

	const char* serializePrefs() const {
		DynamicJsonDocument doc(CALCULATED_PREFS_SIZE);

		doc["Size"] = CALCULATED_PREFS_SIZE;
		doc["Preferences Version"] = PREFERENCES_VERSION;
		doc["Morse Code Char Pairs Version"] = morseCodeCharPairsVersion;

		doc["WiFiSSID"] = wifiSSID;
		doc["WiFiPassword"] = wifiPassword;

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
		//Serial.print(F("MCCP Version: "));
		//Serial.println(morseCodeCharPairsVersion);

		const char* tempSSID = doc["WiFiSSID"];
		setWiFiSSID(copyAndTerminateString(tempSSID, strlen(tempSSID)));

		const char* tempPassword = doc["WiFiPassword"];
		setWiFiPassword(copyAndTerminateString(tempPassword, strlen(tempPassword)));

		return true;
	}
};

#endif
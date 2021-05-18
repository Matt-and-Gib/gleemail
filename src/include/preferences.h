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

	const static unsigned short PREFERENCES_SIZE = 64;

	unsigned short size = 0;
	unsigned short preferencesFileVersion = 1;
	unsigned short morseCodeCharPairsVersion = 2;
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
		char output[size];
		StaticJsonDocument<size> doc;
		//DynamicJsonDocument doc(size);

		doc["Preferences Version"] = preferencesFileVersion;
		doc["Morse Code Char Pairs Version"] = morseCodeCharPairsVersion;
		if(wifiSSID) {
			doc["WiFiSSID"] = wifiSSID;
		}

		if(wifiPassword) {
			doc["WiFiPassword"] = wifiPassword;;
		}

		serializeJson(doc, output);
		return copyString(output, size);
	}

	bool loadSerializedPrefs(const char* input, const unsigned short length) {
		StaticJsonDocument<32> filter;
		filter["Size"] = true;

		StaticJsonDocument<32> sizeDoc;
		deserializeJson(sizeDoc, input, DeserializationOption::Filter(filter));
		size = sizeDoc["Size"];

		Serial.print(F("Size property from SD card Prefs: "));
		Serial.println(size);




		StaticJsonDocument<size> doc;
		DeserializationError error = deserializeJson(doc, input, length);

		if(error) {
			//Serial.print(F("deserializeJson() failed: "));
			//Serial.println(error.f_str());
			DebugLog::getLog().logError(ERROR_CODE::JSON_PREFS_DESERIALIZATION_ERROR);
			return false;
		}

		preferencesFileVersion = doc["Preferences Version"];
		morseCodeCharPairsVersion = doc["Morse Code Char Pairs Version"];

		const char* tempSSID = doc["WiFiSSID"];
		wifiSSID = copyAndTerminateString(tempSSID, strlen(tempSSID));

		const char* tempPassword = doc["WiFiPassword"];
		wifiPassword = copyAndTerminateString(tempPassword, strlen(tempPassword));
	}
};

#endif
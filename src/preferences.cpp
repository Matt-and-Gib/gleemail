#include "include/preferences.h"

#include "include/global.h"

#include <ArduinoJson.h>


Preferences& Preferences::getPrefs() {
	static Preferences prefs; //wrong for singleton pattern!
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


bool Preferences::deserializePrefs(const char* input, const unsigned short length) {
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
			GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::JSON_PREFS_DESERIALIZATION_ERROR);
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
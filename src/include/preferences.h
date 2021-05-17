#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <ArduinoJson.h>


//Remember: Preferences is a singleton! DO NOT waste memory.
class Preferences {
private:
	Preferences() {
		wifiSSID = nullptr;
		wifiPassword = nullptr;
	}
	Preferences(Preferences const&) = delete;
	void operator=(Preferences const&) = delete;

	unsigned short preferencesFileVersion = 1;
	char* wifiSSID;
	char* wifiPassword;
public:
	static Preferences& getPrefs() {
		static Preferences prefs;
		return prefs;
	}

	const char* serializePrefs() const {
		char output[PREFS_DOCUMENT_SIZE];
		StaticJsonDocument<PREFS_DOCUMENT_SIZE> doc;

		doc["Preferences Version"] = preferencesFileVersion;
		if(wifiSSID) {
			doc["WiFiSSID"] = wifiSSID;
		}

		if(wifiPassword) {
			doc["WiFiPassword"] = wifiPassword;;
		}

		serializeJson(doc, output);
		return copyString(output, PREFS_DOCUMENT_SIZE);
	}

	bool loadSerializedPrefs(char* input, const unsigned short length) {
		StaticJsonDocument<48> doc;

		DeserializationError error = deserializeJson(doc, input, length);

		if(error) {
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.f_str());
			return false;
		}

		preferencesFileVersion = doc["Preferences Version"];

		const char* tempSSID = doc["WiFiSSID"];
		wifiSSID = copyString(tempSSID, strlen(tempSSID));

		const char* tempPassword = doc["WiFiPassword"];
		wifiPassword = copyString(tempPassword, strlen(tempPassword));
	}

	const char* getWiFiSSID() const {return wifiSSID;}
	void setWiFiSSID(char* s) {wifiSSID = s;}

	const char* getWiFiPassword() const {return wifiPassword;}
	void setWiFiPassword(char* p) {wifiPassword = p;}
};

#endif
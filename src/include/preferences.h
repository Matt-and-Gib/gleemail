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
	unsigned short morseCodeCharPairsVersion = 2;
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
		doc["Morse Code Char Pairs Version"] = morseCodeCharPairsVersion;
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
		StaticJsonDocument<PREFS_DOCUMENT_SIZE> doc;

		DeserializationError error = deserializeJson(doc, input, length);

		if(error) {
			Serial.print(F("deserializeJson() failed: "));
			Serial.println(error.f_str());
			return false;
		}

		preferencesFileVersion = doc["Preferences Version"];
		morseCodeCharPairsVersion = doc["Morse Code Char Pairs Version"];

		const char* tempSSID = doc["WiFiSSID"];
		wifiSSID = copyAndTerminateString(tempSSID, strlen(tempSSID));

		const char* tempPassword = doc["WiFiPassword"];
		wifiPassword = copyAndTerminateString(tempPassword, strlen(tempPassword));


		Serial.print(F("tempSSID: \'"));
		Serial.print(tempSSID);
		Serial.println(F("\'"));

		Serial.print(F("Length: "));
		Serial.println(strlen(tempSSID));

		Serial.print(F("tempPassword: \'"));
		Serial.print(tempPassword);
		Serial.println(F("\'"));

		Serial.print(F("Length: "));
		Serial.println(strlen(tempPassword));


		Serial.print(F("WIFISSID: \'"));
		Serial.print(wifiSSID);
		Serial.println(F("\'"));

		Serial.print(F("Length: "));
		Serial.println(strlen(wifiSSID));

		Serial.print(F("WIFIPassword: \'"));
		Serial.print(wifiPassword);
		Serial.println(F("\'"));

		Serial.print(F("Length: "));
		Serial.println(strlen(wifiPassword));
	}

	const char* getWiFiSSID() const {return wifiSSID;}
	void setWiFiSSID(char* s) {wifiSSID = s;}

	const char* getWiFiPassword() const {return wifiPassword;}
	void setWiFiPassword(char* p) {wifiPassword = p;}
};

#endif
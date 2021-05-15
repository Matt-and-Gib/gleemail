#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <ArduinoJson.h>


//Remember: Preferences is a singleton! DO NOT waste memory.
class Preferences {
private:
	Preferences() {}
	Preferences(Preferences const&) = delete;
	void operator=(Preferences const&) = delete;

	static const constexpr unsigned short PREFS_DOCUMENT_SIZE = 96;

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
		doc["WiFiSSID"] = wifiSSID;
		doc["WiFiPassword"] = wifiPassword;;

		serializeJson(doc, output);
		return copyString(output, PREFS_DOCUMENT_SIZE);
	}
};

#endif
#ifndef PREFERENCES_H
#define PREFERENCES_H


//Remember: Preferences is a singleton! DO NOT waste memory.
class Preferences {
private:
	Preferences() {}

	const char PREFERENCES_PATH[19] = "GLEEMAIL/PREFS.GMD";

	const static unsigned short PREFERENCES_VERSION = 1;
	const static unsigned short CALCULATED_PREFS_SIZE = 384; //This value represents the size of the current (most up-to-date) version of the preferences file, possibly different than what is on the SD

	unsigned short morseCodeCharPairsVersion = 0;
	char* wifiSSID = nullptr;
	char* wifiPassword = nullptr;
public:
	static Preferences& getPrefs();
	Preferences(Preferences const&) = delete;
	void operator=(Preferences const&) = delete;

	const char* getPrefsPath() const {return PREFERENCES_PATH;}

	const char* getWiFiSSID() const {return wifiSSID;}
	void setWiFiSSID(char* s);

	const char* getWiFiPassword() const {return wifiPassword;}
	void setWiFiPassword(char* p);

	const unsigned short getMorseCodeCharPairsVersion() const {return morseCodeCharPairsVersion;}
	void setMorseCodeCharPairsVersion(const unsigned short v) {morseCodeCharPairsVersion = v;}

	const char* serializePrefs() const;
	bool deserializePrefs(const char* input, const unsigned short length);
};

#endif
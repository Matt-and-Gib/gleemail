#ifndef STORAGE_H
#define STORAGE_H

#include <SD.h>
#include "Arduino.h"

#include "global.h"
#include "preferences.h"


//NOTE: all paths are full caps, and file names must be in 8.3 format
static const constexpr char rootPath[9] = "GLEEMAIL";
static const constexpr char prefsPath[19] = "GLEEMAIL/PREFS.GMD";
static const constexpr char morseCodeCharPairsPath[18] = "GLEEMAIL/MCCP.GMD";

class Storage {
private:
public:
	Storage();
	~Storage();

	bool begin();
	bool clearSavedPrefs(const unsigned short);
	bool loadPrefs();
	bool savePrefs();
	bool loadMorseCodeCharPairs();
	bool saveFile(const char*, const char*);
};

#endif
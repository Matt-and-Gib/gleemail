#ifndef STORAGE_H
#define STORAGE_H

#include <SD.h>
#include "Arduino.h"

#include "global.h"
#include "preferences.h"


static const constexpr char rootPath[9] = "GLEEMAIL";
static const constexpr char prefsPath[19] = "GLEEMAIL/PREFS.GMD";

class Storage {
private:
	//NOTE: all paths are full caps, and file names must be in 8.3 format
	/*static const constexpr char pathSeparator = '/';
	static const constexpr char rootName[9] = "GLEEMAIL";
	static const constexpr char prefsName[11] = "PREFS.GMD";
	static const constexpr char prefsPath[] = rootName + pathSeparator + prefsName;*/

public:
	Storage();
	~Storage();

	bool begin();
	bool loadPrefs();
	bool savePrefs();
	bool clearSavedPrefs(const unsigned short);
};

#endif
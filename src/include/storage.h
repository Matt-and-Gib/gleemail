#ifndef STORAGE_H
#define STORAGE_H

#include <SD.h>
#include <ArduinoJson.h>
#include "Arduino.h"

#include "global.h"
#include "preferences.h"


class Storage {
private:
//NOTE: all paths are full caps, and file names must be in 8.3 format
	/*static const constexpr char pathSeparator = '/';
	static const constexpr char rootName[9] = "glEEmail";
	static const constexpr char prefsName[11] = "prefs.json";
	static const constexpr char prefsPath[] = rootName + pathSeparator + prefsName;*/
	//static const constexpr char prefsPath[] = "";
public:
	Storage();
	~Storage();

	bool begin();
	bool loadPrefs();
	bool savePrefs();
};

#endif
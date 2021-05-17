#include "include/storage.h"


Storage::Storage() {

}


Storage::~Storage() {

}


bool Storage::begin() {
	if(!SD.begin(STORAGE_SLAVE_SELECT_PIN)) {
		return false;
	}

	if(!SD.exists("GLEEMAIL")) {
		SD.mkdir("GLEEMAIL");
		Serial.println(F("Created glEEmail directory in SD card"));
	} else {
		Serial.println(F("glEEmail directory already exists!"));
	}
}


bool Storage::clearSavedPrefs(const unsigned short confirmationNumber) {
	if(confirmationNumber != 1337) {
		return false;
	}

	SD.remove(F("GLEEMAIL/PREFS.GMD"));
	return SD.rmdir(F("GLEEMAIL"));
}


bool Storage::loadPrefs() {
	Serial.println(F("Opening prefs..."));
	File prefsFile = SD.open(F("GLEEMAIL/PREFS.GMD"), FILE_READ);
	if(prefsFile) {
		Serial.println(F("Opened for reading!"));
		const unsigned short dataLength = prefsFile.available();
		char data[dataLength];

		for(unsigned short index = 0; index < dataLength; index += 1) {
			data[index] = prefsFile.read();
		}
		Serial.println(data);

		if(!Preferences::getPrefs().loadSerializedPrefs(data, dataLength)) {
			Serial.println(F("Couldn't deserialize prefs!"));
			return false;
		}
	} else {
		Serial.println(F("Unable to open"));
		return false;
	}
	prefsFile.close();
	Serial.println(F("Closed prefs."));
	return true;
}


bool Storage::savePrefs() {
	Serial.println(F("Opening prefs..."));
	File prefsFile = SD.open(F("GLEEMAIL/PREFS.GMD"), FILE_WRITE);

	if(prefsFile) {
		Serial.println(F("Opened for writing!"));
		prefsFile.println(Preferences::getPrefs().serializePrefs()); //Encrypt me!
	} else {
		Serial.println(F("Unable to open"));
		return false;
	}
	prefsFile.close();
	Serial.println(F("Closed prefs."));

	return true;
}
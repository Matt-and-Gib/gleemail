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
		Serial.println("Created glEEmail directory in SD card");
	} else {
		Serial.println("glEEmail directory already exists!");
	}
}


bool Storage::loadPrefs() {
	Serial.println("Opening prefs...");
	File prefsFile = SD.open("GLEEMAIL/PREFS.GMD", FILE_READ);
	if(prefsFile) {
		Serial.println("Opened for reading!");
		while(prefsFile.available()) {
			Serial.write(prefsFile.read());
		}
	} else {
		Serial.println("Unable to open");
		return false;
	}
	prefsFile.close();
	Serial.println("Closed prefs.");
	return true;
}


bool Storage::savePrefs() {
	Serial.println("Opening prefs...");
	File prefsFile = SD.open("GLEEMAIL/PREFS.GMD", FILE_WRITE);

	if(prefsFile) {
		Serial.println("Opened for writing!");
		prefsFile.println(Preferences::getPrefs().serializePrefs());
	} else {
		Serial.println("Unable to open");
		return false;
	}
	prefsFile.close();
	Serial.println("Closed prefs.");

	return true;
}
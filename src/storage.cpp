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
		Serial.println(F("glEEmail directory exists!"));
	}
}


bool Storage::clearSavedPrefs(const unsigned short confirmationNumber) {
	if(confirmationNumber != 1337) {
		return false;
	}

	return SD.remove(F("GLEEMAIL/PREFS.GMD"));

	/*if(!SD.remove(F("GLEEMAIL/PREFS.GMD"))) {
		Serial.println(F("Couldn't delete prefs file"));
	}

	return SD.rmdir(F("GLEEMAIL"));*/
}


bool Storage::loadPrefs() {
	Serial.println(F("Opening prefs..."));
	File prefsFile = SD.open("GLEEMAIL/PREFS.GMD", FILE_READ);
	if(prefsFile) {
		Serial.println(F("Opened for reading!"));
		
		unsigned short dataLength = 0;
		char data[PREFS_DOCUMENT_SIZE];
		while(prefsFile.available()) {
			data[dataLength] = prefsFile.read();
			dataLength += 1;
		}

		prefsFile.close();

		Serial.print(F("Data length: "));
		Serial.println(dataLength);

		Serial.print(F("Data: "));
		Serial.println(data);

		if(!Preferences::getPrefs().loadSerializedPrefs(data, dataLength)) {
			Serial.println(F("Couldn't deserialize prefs!"));
			return false;
		}
	} else {
		Serial.println(F("Unable to open"));
		prefsFile.close();
		return false;
	}

	Serial.println(F("Closed prefs."));
	return true;
}


bool Storage::savePrefs() {
	Serial.println(F("Opening prefs..."));
	File prefsFile = SD.open(F("GLEEMAIL/PREFS.GMD"), FILE_WRITE);

	if(prefsFile) {
		Serial.println(F("Opened for writing!"));
		const char* prefsData = Preferences::getPrefs().serializePrefs();
		Serial.print(F("Saving: "));
		Serial.println(prefsData);
		Serial.println(F("done"));

		prefsFile.println(prefsData); //Encrypt me!
		delete[] prefsData;
	} else {
		Serial.println(F("Unable to open"));
		return false;
	}
	prefsFile.close();
	Serial.println(F("Closed prefs."));

	return true;
}
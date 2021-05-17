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

	return SD.remove("GLEEMAIL/PREFS.GMD");

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
		char data[prefsFile.size() + 1];
		while(prefsFile.available()) {
			/*if(prefsFile.peek() == '\r' || prefsFile.peek() == '\n') {
				Serial.println(F("stopping at line break"));
				break;
			}*/
			data[dataLength] = prefsFile.read();
			dataLength += 1;
		}

		prefsFile.close();
		data[dataLength] = '\0';

		Serial.print(F("Data:"));
		Serial.println(data);

		if(!Preferences::getPrefs().loadSerializedPrefs(data, dataLength)) {
			Serial.println(F("Couldn't deserialize prefs!"));
			return false;
		}

		Serial.println(F("Closed prefs."));
		return true;
	} else {
		Serial.println(F("Unable to open"));
		prefsFile.close();
		return false;
	}
}


bool Storage::savePrefs() {
	Serial.println(F("Opening prefs..."));
	File prefsFile = SD.open(F("GLEEMAIL/PREFS.GMD"), O_WRITE | O_CREAT);

	if(prefsFile) {
		Serial.println(F("Opened for writing!"));
		const char* prefsData = Preferences::getPrefs().serializePrefs();
		Serial.print(F("Saving: "));
		Serial.println(prefsData);
		Serial.println(F("done"));

		prefsFile.print(prefsData); //Encrypt me!
		prefsFile.close();

		delete[] prefsData;

		Serial.println(F("Closed prefs."));
		return true;
	} else {
		Serial.println(F("Unable to open"));
		prefsFile.close();
		return false;
	}
}
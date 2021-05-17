#include "include/storage.h"


Storage::Storage() {

}


Storage::~Storage() {

}


bool Storage::begin() {
	if(!SD.begin(STORAGE_SLAVE_SELECT_PIN)) {
		return false;
	}

	if(!SD.exists(rootPath)) {
		SD.mkdir(rootPath);
	}
}


bool Storage::clearSavedPrefs(const unsigned short confirmationNumber) {
	if(confirmationNumber != 1337) {
		return false;
	}

	return SD.remove(prefsPath);
}


bool Storage::loadPrefs() {
	File prefsFile = SD.open(prefsPath, FILE_READ);
	if(prefsFile) {
		unsigned short dataLength = 0;
		char data[prefsFile.size() + 1];
		if(prefsFile.size() + 1 > PREFS_DOCUMENT_SIZE) {
			DebugLog::getLog().logError(ERROR_CODE::STORAGE_PREFS_FILE_SIZE_GREATER_THAN_PREFS_DOCUMENT_SIZE);
		}

		while(prefsFile.available()) {
			data[dataLength] = prefsFile.read();
			dataLength += 1;
		}

		prefsFile.close();
		data[dataLength] = '\0';

		if(!Preferences::getPrefs().loadSerializedPrefs(data, dataLength)) {
			return false;
		}

		return true;
	} else {
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
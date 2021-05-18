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
	File prefsFile = SD.open(prefsPath, O_WRITE | O_CREAT);

	if(prefsFile) {
		const char* prefsData = Preferences::getPrefs().serializePrefs();
		prefsFile.print(prefsData); //Encrypt me!
		prefsFile.close();

		delete[] prefsData;
		return true;
	} else {
		prefsFile.close();
		return false;
	}
}

/*
	{
		"size": 3072	
	}

	StaticJsonDocument<16> filter;
	filter["size"] = true;

	StaticJsonDocument<16> doc;
	deserializeJson(doc, input_json, DeserializationOption::Filter(filter));
*/


bool Storage::loadMorseCodeCharPairs() {
	File mccpFile = SD.open(morseCodeCharPairsPath, FILE_READ);
	if(mccpFile) {
		unsigned short dataLength = 0;
		char data[mccpFile.size() + 1];

		while(mccpFile.available()) {
			data[dataLength++] = mccpFile.read();
		}

		mccpFile.close();
		data[dataLength] = '\0';

		StaticJsonDocument<16> filter;
		filter["size"] = true;

		StaticJsonDocument<16> sizeDoc;
		deserializeJson(sizeDoc, data, DeserializationOption::Filter(filter));
		const unsigned short mccpSize = sizeDoc["size"];

		DynamicJsonDocument mccpDoc(mccpSize);
		DeserializationError error = deserializeJson(mccpDoc, data);

		if(error) {
			Serial.println(error.f_str());
		}

		const char* letter;
		const char* phrase;
		for (ArduinoJson::JsonObject elem : mccpDoc["morsecodetreedata"].as<ArduinoJson::JsonArray>()) {
			letter = elem["symbol"];
			phrase = elem["phrase"];
			Serial.print(F("Adding: "));
			Serial.print(phrase);
			Serial.print(F(" : "));
			Serial.println(letter);
		}
	} else {
		mccpFile.close();
		return false;
	}
}
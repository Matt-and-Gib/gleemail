#include "include/storage.h"
#include <SPI.h>
#include "SdFat.h"
#include <Arduino.h>


bool Storage::begin() {
	if(!sd) {
		sd = new SdFat32;
		if(sd != nullptr) {
			//failure: sd.initErrorHalt(&Serial);
			return sd->begin(SLAVE_SELECT_PIN);
		} else {
			return false;
		}
	} else {
		return true; //log warning?
	}
}


bool Storage::writeFile(const char* data, const char* filePath) {
	Serial.print(F("filePath: "));
	Serial.println(filePath);

	Serial.print(F("data: "));
	Serial.println(data);

	if(!sd) {
		Serial.println(F("!sd"));
		return false;
	}

	File32 item; //make member instead?
	if(!item.openRoot(sd->vol())) {
		Serial.println(F("!item.openRoot(sd->vol())"));
		return false;
	}

	if(!item.exists(GLEEMAIL_ROOT_PATH)) {
		Serial.println(F("!item.exists(ROOT_PATH)"));
		File32 createdFolder;
		if(!createdFolder.mkdir(&item, GLEEMAIL_ROOT_PATH, true)) {
			Serial.println(F("item.mkdir FAILED!"));
		} else {
			item.sync();
		}
	}
	item.close();

	if(!item.open(filePath, O_RDWR | O_CREAT | O_TRUNC)) {
		Serial.println(F("itemOpen failed"));
		return false;
	}

	const short wroteLength = item.write(data); //make member instead?
	item.sync();
	item.close();

	return wroteLength != -1;
}


const char* Storage::readFile(const char* filePath) {
	if(!sd) {
		return nullptr;
	}

	File32 item; //make member instead?
	if(!item.open(filePath)) {
		return nullptr;
	}

	dataLength = 0;

	char* fileData = new char[item.fileSize() + 1];
	while(item.available()) {
		fileData[dataLength++] = item.read();
	}

	item.close();
	fileData[dataLength] = '\0';

	return fileData;
}


unsigned int Storage::lastReadFileLength() const {
	return dataLength;
}


bool Storage::clearFile(const char* filePath) {
	return true;
}


bool Storage::eraseAll(const unsigned int confirmationCode) {
	if(confirmationCode != 133769) {
		return false;
	}

	if(!sd) {
		begin();
	}

	File32 item; //make member instead?
	if(!item.open(GLEEMAIL_ROOT_PATH)) {
		Serial.println(F("eraseAll: Unable to open root path."));
		return false; //maybe return true if the root directory doesn't exist?
	}

	bool removeSuccess = false;

	if(item.isDirectory()) {
		Serial.println(F("eraseAll: root is directory"));
		removeSuccess = item.rmRfStar(); //This function should not be used to delete the 8.3 version of a directory that has a long name.
	} else {
		Serial.println(F("eraseAll: root is a file"));
		removeSuccess = item.remove(); //This function should not be used to delete the 8.3 version of a file that has a long name. For example if a file has the long name "New Text Document.txt" you should not delete the 8.3 name "NEWTEX~1.TXT".
	}

	item.close();
	return removeSuccess;
}


void Storage::printAll() {
	return;
}
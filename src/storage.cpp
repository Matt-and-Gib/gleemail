#include "include/storage.h"
#include "include/global.h"
#include <SD.h>


Storage::Storage() {

}


Storage::~Storage() {

}


bool Storage::begin() {
	if(!SD.begin(STORAGE_SLAVE_SELECT_PIN)) {
		return false;
	}

	if(!SD.exists(ROOT_PATH)) {
		SD.mkdir(ROOT_PATH);
	}

	return true;
}


bool Storage::clearFile(const char* filePath) {
	return SD.remove(filePath);
}


bool Storage::writeFile(const char* data, const char* filePath) { //The data that is sent into saveFile() ABSOLUTELY MUST BE TERMINATED!
	File saveToFile = SD.open(filePath, O_WRITE | O_CREAT);

	if(saveToFile) {
		saveToFile.print(data);
		saveToFile.close();

		return true;
	} else {
		return false;
	}
}


const char* Storage::readFile(const char* filePath) { //REMEMBER TO DELETE! This is on the heap
	dataLength = 0;
	File readFromFile = SD.open(filePath, FILE_READ);
	if(readFromFile) {
		char* data = new char[readFromFile.size() + 1];

		while(readFromFile.available()) {
			data[dataLength++] = readFromFile.read();
		}

		readFromFile.close();
		data[dataLength] = '\0';

		return data;
	} else {
		return nullptr;
	}
}


unsigned int Storage::lastReadFileLength() const {
	return dataLength; //== 0 ? 0 : dataLength + 1;
}


bool Storage::eraseAll(const unsigned int confirmationCode) {
	//If you're looking for the confirmation code, make sure that you understand this fuction will erase everything on the SD card.
	if(confirmationCode != 133769) {
		return false;
	}

	//TODO: recursively search and destory from gleemail root instead of listing specific files.
	clearFile(PREFS_PATH);
	clearFile(MORSE_CODE_CHAR_PAIRS_PATH);

	return true;
}
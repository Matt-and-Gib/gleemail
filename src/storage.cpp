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

	return true;
}


bool Storage::clearFile(const char* filePath) {
	return SD.remove(filePath);
}


bool Storage::writeFile(const char* data, const char* filePath, const bool encrypt) { //The data that is sent into saveFile() ABSOLUTELY MUST BE TERMINATED!
	File saveToFile = SD.open(filePath, O_WRITE | O_CREAT);

	if(saveToFile) {
		if(encrypt) {
			char* encryptedData = /*encrypt(*/copyString(data, 0)/*)*/; //The zero in copyString is a placeholder. It is NOT CORRECT
			saveToFile.print(encryptedData);
			delete[] encryptedData;
		} else {
			saveToFile.print(data);
		}
		saveToFile.close();
		return true;
	} else {
		return false;
	}
}


const char* Storage::readFile(const char* filePath, const bool decrypt) { //REMEMBER TO DELETE! This is on the heap
	dataLength = 0;
	File readFromFile = SD.open(filePath, FILE_READ);
	if(readFromFile) {
		char* data = new char[readFromFile.size() + 1];

		while(readFromFile.available()) {
			data[dataLength++] = readFromFile.read();
		}

		readFromFile.close();
		data[dataLength] = '\0';

		if(decrypt) {
			return /*decrypt(*/data/*, decryptionSecret)*/;
		} else {
			return data;
		}
	} else {
		return nullptr;
	}
}
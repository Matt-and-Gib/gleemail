#include "include/storage.h"
#include "include/global.h"
#include <SD.h>

using namespace SDLib;


Storage::Storage() {

}


Storage::~Storage() {

}


bool Storage::begin() {
	if(!SD.begin(STORAGE_SLAVE_SELECT_PIN)) {
		return false;
	}

	if(!SD.exists(getRootPath())) {
		SD.mkdir(getRootPath());
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


void Storage::recursiveErase(SDLib::File& root, const char* rootPath = nullptr) {
	char path[80] = {0};
	if(rootPath) {
		strcat(path, rootPath);
		strcat(path, "/\0");
	}
	strcat(path, root.name());

	File item;
	while(item = root.openNextFile()) {
		if(item.isDirectory()) {
			recursiveErase(item, path);

			char eraseDirectoryPath[80] = {0};
			strcat(eraseDirectoryPath, path);
			strcat(eraseDirectoryPath, "/\0");
			strcat(eraseDirectoryPath, item.name());
			SD.rmdir(eraseDirectoryPath);
		} else {
			char eraseFilePath[80] = {0};
			strcat(eraseFilePath, path);
			strcat(eraseFilePath, "/\0");
			strcat(eraseFilePath, item.name());
			SD.remove(eraseFilePath);
		}
	}
}


bool Storage::eraseAll(const unsigned int confirmationCode) {
	//If you're looking for the confirmation code, make sure that you understand this fuction will erase all GLEEMAIL files and folders on the SD card.
	if(confirmationCode != 133769) {
		return false;
	}

	File rootFile = SD.open(getRootPath(), FILE_READ);
	recursiveErase(rootFile);
	SD.rmdir(getRootPath());
	
	return true;
}


void Storage::recursivePrint(SDLib::File& root, const char* rootPath = nullptr) {
	char path[80] = {0};
	if(rootPath) {
		strcat(path, rootPath);
		strcat(path, "/\0");
	}
	strcat(path, root.name());

	File item;
	while(item = root.openNextFile()) {
		if(item.isDirectory()) {
			recursivePrint(item, path);

			char printDirectoryPath[80] = {0};
			strcat(printDirectoryPath, path);
			strcat(printDirectoryPath, "/\0");
			strcat(printDirectoryPath, item.name());

			Serial.println(printDirectoryPath);
		} else {
			char printFilePath[80] = {0};
			strcat(printFilePath, path);
			strcat(printFilePath, "/\0");
			strcat(printFilePath, item.name());

			Serial.println(printFilePath);
		}
	}
}


void Storage::printAll() {
	if(SD.exists(getRootPath())) {
		File rootFile = SD.open(getRootPath(), FILE_READ);
		recursivePrint(rootFile);
		Serial.println(getRootPath());
	}
}
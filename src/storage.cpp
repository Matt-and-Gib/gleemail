#include "include/storage.h"
#include "include/global.h"
#include "SdFat.h"
#include <SPI.h>


bool Storage::begin() {
	if(!sd) {
		sd = new SdFat32;
		if(sd != nullptr) {
			if(sd->begin(SLAVE_SELECT_PIN)) {
				return true;
			} else {
				delete sd;
				sd = nullptr;
				return false;
			}
		} else {
			return false; //This false occurs if sd allocation fails. This should NEVER happen, and if it does, you have bigger (and probably hardware) problems, buddy! It is here to prevent returning true should this be the case.
		}
	} else {
		GLEEMAIL_DEBUG::DebugLog::getLog().logWarning(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_ALREADY_INITIALIZED);
		return true;
	}
}


bool Storage::writeFile(const char* data, const char* filePath) {
	if(!sd) {
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_UNINITIALIZED_WRITE);
		return false;
	}

	File32 item;
	if(!item.openRoot(sd->vol())) {
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_ROOT_FS_ACCESS_FAILED);
		return false;
	}

	if(!item.exists(GLEEMAIL_ROOT_PATH)) {
		File32 createdFolder;
		if(!createdFolder.mkdir(&item, GLEEMAIL_ROOT_PATH, true)) {
			GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_CREATE_GLEEMAIL_ROOT_FAILED);
			//item.close();
			return false;
		} else {
			item.sync();
		}
	}
	item.close();

	if(!item.open(filePath, O_RDWR | O_CREAT | O_TRUNC)) {
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_WRITE_OPEN_FAILED);
		return false;
	}

	const short wroteLength = item.write(data);
	item.sync();
	item.close();

	return wroteLength != -1;
}


const char* Storage::readFile(const char* filePath) {
	if(!sd) {
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_UNINITIALIZED_READ);
		return nullptr;
	}

	File32 item;
	if(!item.open(filePath)) {
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_READ_OPEN_FAILED);
		return nullptr;
	}

	dataLength = 0;

	char* fileData = new char[item.fileSize() + TERMINATOR];
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


bool Storage::eraseFile(const char* removeAtPath) {
	if(!sd) {
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_UNINITIALIZED_ERASE);
		return false;
	}

	return sd->remove(removeAtPath);
}


bool Storage::eraseAll(const unsigned int confirmationCode) {
	if(confirmationCode != 133769) {
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_UNCONFIRMED_ERASE);
		return false;
	}

	if(!sd) {
		GLEEMAIL_DEBUG::DebugLog::getLog().logWarning(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_UNINITIALIZED_ERASE_ALL);
		begin();
	}

	File32 item;
	if(!item.open(GLEEMAIL_ROOT_PATH)) {
		GLEEMAIL_DEBUG::DebugLog::getLog().logWarning(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_OPEN_GLEEMAIL_ROOT_FOR_ERASE_FAILED);
		return sd->exists(GLEEMAIL_ROOT_PATH);
	}

	bool removeSuccess = false;

	if(item.isDirectory()) {
		removeSuccess = item.rmRfStar(); //This function should not be used to delete the 8.3 version of a directory that has a long name.
	} else {
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_GLEEMAIL_ROOT_IS_FILE);
		removeSuccess = sd->remove(GLEEMAIL_ROOT_PATH); //item.remove(); //This function should not be used to delete the 8.3 version of a file that has a long name. For example if a file has the long name "New Text Document.txt" you should not delete the 8.3 name "NEWTEX~1.TXT".
	}
	item.close();
	return removeSuccess;
}
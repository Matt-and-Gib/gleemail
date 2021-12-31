#include "include/storage.h"
#include "include/global.h"
#include "SdFat.h"
#include <SPI.h>

#include "include/keyvaluepair.h"
#include "include/queue.h"


Storage::Storage() : StartupCodeHandler() {

}


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
			return false; //This false occurs if SD allocation fails. This should NEVER happen, and if it does, you have bigger (and probably hardware) problems! It is here to prevent returning true should this be the case.
		}
	} else {
		GLEEMAIL_DEBUG::DebugLog::getLog().logWarning(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_ALREADY_INITIALIZED);
		return true;
	}
}


void Storage::registerNewStartupCodes(Queue<KVPair<char, StartupCodeHandlerInfo*>>& startupCodeHandlers) {
	startupCodeHandlers.enqueue(new KVPair<char, StartupCodeHandlerInfo*>(RESET_STARTUP_CODE, new StartupCodeHandlerInfo(this, reinterpret_cast<bool (StartupCodeHandler::*)(void)>(&Storage::resetStartupCodeReceived))));
}


void Storage::startupCodeReceived(bool (StartupCodeHandler::*memberFunction)(void)) {
	(this->*reinterpret_cast<bool (Storage::*)(void)>(memberFunction))();
}


bool Storage::writeFile(const char* data, const char* filePath) const {
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
			item.close();
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


bool Storage::eraseFile(const char* removeAtPath) const {
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
		removeSuccess = item.rmRfStar();
	} else {
		GLEEMAIL_DEBUG::DebugLog::getLog().logError(GLEEMAIL_DEBUG::ERROR_CODE::STORAGE_GLEEMAIL_ROOT_IS_FILE);
		removeSuccess = sd->remove(GLEEMAIL_ROOT_PATH);
	}
	item.close();
	return removeSuccess;
}
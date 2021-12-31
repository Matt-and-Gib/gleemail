#ifndef STORAGE_H
#define STORAGE_H

#include "startupcodehandler.h"

class SdFat32;


class Storage final : public StartupCodeHandler {
private:
	const unsigned short SLAVE_SELECT_PIN = 10;

	const char RESET_STARTUP_CODE = 'R';
	bool resetStartupCodeReceived() {return eraseAll(133769);}
	
	const char GLEEMAIL_ROOT_PATH[9] = "GLEEMAIL";

	unsigned int dataLength = 0;

	SdFat32* sd = nullptr;
public:
	Storage();

	bool begin();

	void registerNewStartupCodes(StartupCodeHandler*, Queue<KVPair<char, StartupCodeHandlerInfo*>>&) const;
	void startupCodeReceived(bool (StartupCodeHandler::*)(void));

	const char* getRootPath() const {return GLEEMAIL_ROOT_PATH;}
	
	bool writeFile(const char* data, const char* filePath) const;

	const char* readFile(const char* filePath); //Remember to delete! Stores on heap
	unsigned int lastReadFileLength() const {return dataLength;}

	bool eraseFile(const char* removeAtPath) const;
	bool eraseAll(const unsigned int confirmationCode);
};

#endif
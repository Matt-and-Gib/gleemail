#ifndef STORAGE_H
#define STORAGE_H

#include "startupcodehandler.h"


#include "Arduino.h" //delete me!

class SdFat32;


class Storage final : public StartupCodeHandler {
private:
	const unsigned short SLAVE_SELECT_PIN = 10;

	const char RESET_STARTUP_CODE = 'R';
	bool resetStartupCodeReceived() {Serial.println(F("reset SD called!"));/*return eraseAll(133769);*/}

	const char GLEEMAIL_ROOT_PATH[9] = "GLEEMAIL";

	unsigned int dataLength = 0;

	SdFat32* sd = nullptr;
public:
	explicit Storage() = default;
	~Storage() = default;

	bool begin();

	void registerNewStartupCodes(Queue<KVPair<char, StartupCodeHandlerData*>>&, StartupCodeHandler* const) override;
	void startupCodeReceived(bool (StartupCodeHandler::*)(void)) override;

	const char* getRootPath() const {return GLEEMAIL_ROOT_PATH;}
	
	bool writeFile(const char* data, const char* filePath) const;

	const char* readFile(const char* filePath); //Remember to delete! Stores on heap
	unsigned int lastReadFileLength() const {return dataLength;}

	bool eraseFile(const char* removeAtPath) const;
	bool eraseAll(const unsigned int confirmationCode);
};

#endif
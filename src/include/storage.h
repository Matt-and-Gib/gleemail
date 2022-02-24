#ifndef STORAGE_H
#define STORAGE_H


#ifdef UNIT_TEST_MODE
#include mock_startupcodehandler.h
#else
#include "startupcodehandler.h"
#endif


class SdFat32;


class Storage final : public StartupCodeHandler {
private:
	const unsigned short SLAVE_SELECT_PIN = 10;

	const char RESET_STARTUP_CODE = 'R';
	void resetStartupCodeReceived() {eraseAll(133769);}

	const char GLEEMAIL_ROOT_PATH[9] = "GLEEMAIL";

	unsigned int dataLength = 0;

	SdFat32* sd = nullptr;
public:
	explicit Storage() = default;
	Storage(const Storage&) = delete;
	Storage(Storage&&) = delete;
	Storage& operator=(const Storage&) = delete;
	Storage& operator=(Storage&&) = delete;
	~Storage() = default;

	bool begin();

	void registerNewStartupCodes(Queue<KVPair<const char&, StartupCodeHandlerData* const>>&) override;
	void startupCodeReceived(void (StartupCodeHandler::*)(void)) override;

	const char* getRootPath() const {return GLEEMAIL_ROOT_PATH;}
	
	bool writeFile(const char* data, const char* filePath) const;

	const char* readFile(const char* filePath); //Remember to delete! Stores on heap
	unsigned int lastReadFileLength() const {return dataLength;}

	bool eraseFile(const char* removeAtPath) const;
	bool eraseAll(const unsigned int confirmationCode);
};

#endif
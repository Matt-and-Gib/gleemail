#ifndef STORAGE_H
#define STORAGE_H

#ifdef UNIT_TEST_MODE
//include moc_SD.h
//include moc_global.h
#else
#include <SdFat.h>
#include "global.h"
#endif


class Storage {
private:
	unsigned int dataLength = 0;
public:
	Storage();
	~Storage();

	bool begin();
	bool clearFile(const char* filePath);

	bool writeFile(const char* data, const char* filePath, const bool encrypt = false);

	const char* readFile(const char* filePath, const bool decrypt = false); //Remember to delete! Stores on heap
	unsigned int lastReadFileLength() const {return dataLength == 0 ? 0 : dataLength + 1;}
};

#endif
#ifndef STORAGE_H
#define STORAGE_H

#include <SD.h>

#include "global.h"


class Storage {
private:
	unsigned int dataLength = 0;
public:
	Storage();
	~Storage();

	bool begin();
	bool clearFile(const char* filePath);

	bool writeFile(const char* data, const char* filePath, const bool encrypt = false);

	char* readFile(const char* filePath, const bool decrypt = false);
	const unsigned int lastReadFileLength() const {return dataLength == 0 ? 0 : dataLength + 1;}
};

#endif
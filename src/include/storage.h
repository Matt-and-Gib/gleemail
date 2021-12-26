#ifndef STORAGE_H
#define STORAGE_H


class SdFat32;


class Storage {
private:
	const unsigned short SLAVE_SELECT_PIN = 10;
	const char RESET_CODE = 'R';
	const char GLEEMAIL_ROOT_PATH[9] = "GLEEMAIL";

	unsigned int dataLength = 0;

	SdFat32* sd;
public:
	bool begin();

	const char* getRootPath() const {return GLEEMAIL_ROOT_PATH;}
	
	bool writeFile(const char* data, const char* filePath);

	const char* readFile(const char* filePath); //Remember to delete! Stores on heap
	unsigned int lastReadFileLength() const;

	bool eraseFile(const char* removeAtPath);
	bool eraseAll(const unsigned int confirmationCode);
};

#endif
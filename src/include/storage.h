#ifndef STORAGE_H
#define STORAGE_H

namespace SDLib {
	class File;
}


class Storage {
private:
	const char ROOT_PATH[9] = "GLEEMAIL";

	unsigned int dataLength = 0;

	void recursiveErase(SDLib::File&, const char*);
	void recursivePrint(SDLib::File&, const char*);
public:
	bool begin();

	const char* getRootPath() const {return ROOT_PATH;}
	
	bool writeFile(const char* data, const char* filePath);

	const char* readFile(const char* filePath); //Remember to delete! Stores on heap
	unsigned int lastReadFileLength() const;

	bool clearFile(const char* filePath);
	bool eraseAll(const unsigned int confirmationCode);

	void printAll();
};

#endif
#ifndef STORAGE_H
#define STORAGE_H

class Storage {
private:
	unsigned int dataLength = 0;
public:
	Storage();
	~Storage();

	bool begin();
	bool clearFile(const char* filePath);

	bool writeFile(const char* data, const char* filePath);

	const char* readFile(const char* filePath); //Remember to delete! Stores on heap
	unsigned int lastReadFileLength() const;

	bool eraseAll(const unsigned int confirmationCode);
};

#endif
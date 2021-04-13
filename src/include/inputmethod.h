#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H

#include "global.h"


class InputMethod {
private:
	unsigned long lastDebounceTime = 0;

	bool messageComplete = false;
	bool messageRetrieved = false;
	char *userMessage;
	unsigned short int userMessageFirstEmptyIndex = 0;
public:
	InputMethod();
	~InputMethod();

	virtual bool setNetworkData(const char*) = 0;
	virtual const char* getServerAddress() const = 0;
	virtual const char* const* getRequestHeaders() const = 0;

	virtual Pin **getPins() = 0;
	virtual void processInput(const unsigned long) = 0;

	virtual unsigned short getDebounceThreshold() = 0;

	unsigned long getLastDebounceTime() const {return lastDebounceTime;}
	void setLastDebounceTime(const unsigned long t) {lastDebounceTime = t;}

	void getuserMessage(char *);
	void commitMessage();
	bool isMessageReady() const {return messageComplete;}
	bool messageNotEmpty() const {return userMessage[0] != '\0';}
	bool isLastCharSpace() const;

	void pushCharacterToMessage(const char c);
};

#endif
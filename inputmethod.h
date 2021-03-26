#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H

#include "global.h"


class InputMethod {
public:
	InputMethod();
	~InputMethod();

	virtual Pin **getPins() = 0;
	virtual void processInput(const unsigned long) = 0;

	virtual unsigned short getDebounceThreshold() = 0;
	const unsigned long getLastDebounceTime() const {return lastDebounceTime;}
	void setLastDebounceTime(const unsigned long t) {lastDebounceTime = t;}

	void getMessageToSend(char *);
	void commitMessage();
	bool isMessageReady() const {return messageComplete;}
	bool messageNotEmpty() const {return messageToSend[0] != '\0';}
	bool isLastCharSpace() const;

	void pushCharacterToMessage(const char c);

	void pushErrorCode(const ERROR_CODE);
	ERROR_CODE **getErrorCodes() {return errorCodes;}
private:
	static constexpr short MAX_ERROR_CODES = 128;
	ERROR_CODE *errorCodes[MAX_ERROR_CODES];
	ERROR_CODE *tempCode;

	unsigned long lastDebounceTime = 0;

	bool messageComplete = false;
	bool messageRetrieved = false;
	char *messageToSend;
	unsigned short int messageToSendFirstEmptyIndex = 0;
};

#endif
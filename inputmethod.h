#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H

#include "global.h"


class InputMethod {
public:
	virtual Pin **getPins() = 0;
	virtual void processInput(const unsigned long) = 0;

	void getMessageToSend(char *);
	void commitMessage();
	bool isMessageReady() const {return messageComplete;}
	bool messageNotEmpty() const {return messageToSend[0] != '\0';}
	bool isLastCharSpace() const;

	void pushCharacterToMessage(const char c);
private:
	bool messageComplete = false;
	bool messageRetrieved = false;
	char messageToSend[MAX_MESSAGE_LENGTH];
	unsigned short int messageToSendFirstEmptyIndex = 0;
};

#endif
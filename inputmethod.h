#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H

#include "global.h"


class InputMethod {
public:
	virtual Pin **getPins() = 0;
	virtual void processInput(const unsigned long) = 0;

	void getMessageToSend(char *);
	void commitMessage() {messageComplete = true;}
	bool isMessageReady() const {return messageComplete;}

	void pushCharacterToMessage(const char c);
private:
	bool messageComplete = false;
	bool messageRetrieved = false;
	char messageToSend[MAX_MESSAGE_LENGTH];
};

#endif
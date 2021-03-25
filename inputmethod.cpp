#include "inputmethod.h"


InputMethod::InputMethod() {
	messageToSend = new char[MAX_MESSAGE_LENGTH];
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		messageToSend[i] = '\0';
	}

	for(int i = 0; i < MAX_ERROR_CODES; i += 1) {
		errorCodes[i] = new ERROR_CODE(ERROR_CODE::NONE);
	}
}


InputMethod::~InputMethod() {
	delete[] messageToSend;
	for(int i = 0; i < MAX_ERROR_CODES; i += 1) {
		delete errorCodes[i];
	}
}


void InputMethod::pushErrorCode(const ERROR_CODE code) {
	unsigned short index = 0;
	tempCode = errorCodes[index];
	while(*tempCode != ERROR_CODE::NONE) {
		tempCode = errorCodes[++index];
	}

	if(index < MAX_ERROR_CODES) {
		*errorCodes[index] = code;
	}
}


void InputMethod::getMessageToSend(char *messageOut) {
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		messageOut[i] = messageToSend[i];
		messageToSend[i] = '\0';
	}

	messageToSendFirstEmptyIndex = 0;
	messageComplete = false;
}


void InputMethod::pushCharacterToMessage(const char c) {
	messageComplete = false;

	if(messageToSendFirstEmptyIndex < MAX_MESSAGE_LENGTH) {
		messageToSend[messageToSendFirstEmptyIndex] = c;
		messageToSendFirstEmptyIndex += 1;
	} else {
		//Possibly log error (this should not happen)
		commitMessage();
	}
}


void InputMethod::commitMessage() {
	messageComplete = true;
	//trim whitespace from message
}


bool InputMethod::isLastCharSpace() const {
	return messageToSendFirstEmptyIndex == 0 ? false : (messageToSend[messageToSendFirstEmptyIndex - 1] == ' ');
}
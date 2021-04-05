#include "include/inputmethod.h"


InputMethod::InputMethod() {
	messageToSend = new char[MAX_MESSAGE_LENGTH];
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		messageToSend[i] = '\0';
	}
}


InputMethod::~InputMethod() {
	delete[] messageToSend;
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
	if(c != CANCEL_CHAR) {
		messageComplete = false;

		if(messageToSendFirstEmptyIndex < MAX_MESSAGE_LENGTH) {
			messageToSend[messageToSendFirstEmptyIndex] = c;
			messageToSendFirstEmptyIndex += 1;
		} else {
			//Possibly log error (this should not happen)
			commitMessage();
		}
	}
}


void InputMethod::commitMessage() {
	messageComplete = true;
	//trim whitespace from message
}


bool InputMethod::isLastCharSpace() const {
	return messageToSendFirstEmptyIndex == 0 ? false : (messageToSend[messageToSendFirstEmptyIndex - 1] == ' ');
}
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

			if(messageToSendFirstEmptyIndex == MAX_MESSAGE_LENGTH) {
				DebugLog::getLog().logError(MORSE_MESSAGE_TO_SEND_REACHED_MAX_MESSAGE_LENGTH, false);
				commitMessage();
			}
		} else {
			DebugLog::getLog().logError(MORSE_MESSAGE_TO_SEND_EXCEEDED_MAX_MESSAGE_LENGTH);
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
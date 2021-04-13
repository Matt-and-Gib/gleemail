#include "include/inputmethod.h"


InputMethod::InputMethod() {
	userMessage = new char[MAX_MESSAGE_LENGTH];
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		userMessage[i] = '\0';
	}
}


InputMethod::~InputMethod() {
	delete[] userMessage;
}


void InputMethod::getuserMessage(char *messageOut) {
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		messageOut[i] = userMessage[i];
		userMessage[i] = '\0';
	}

	userMessageFirstEmptyIndex = 0;
	messageComplete = false;
}


void InputMethod::pushCharacterToMessage(const char c) {
	if(c != CANCEL_CHAR) {
		messageComplete = false;

		if(userMessageFirstEmptyIndex < MAX_MESSAGE_LENGTH) {
			userMessage[userMessageFirstEmptyIndex] = c;
			userMessageFirstEmptyIndex += 1;

			if(userMessageFirstEmptyIndex == MAX_MESSAGE_LENGTH) {
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
	return userMessageFirstEmptyIndex == 0 ? false : (userMessage[userMessageFirstEmptyIndex - 1] == ' ');
}
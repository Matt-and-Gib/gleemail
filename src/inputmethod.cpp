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


void InputMethod::clearUserMessage() {
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		userMessage[i] = '\0';
	}
}


void InputMethod::updateMessageOutBuffer(char *messageOut) {
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		messageOut[i] = userMessage[i];
	}
}


void InputMethod::getUserMessage(char *messageOut) {
	updateMessageOutBuffer(messageOut);
	clearUserMessage();

	userMessageFirstEmptyIndex = 0;
	messageChanged = false;
	messageComplete = false;
}


void InputMethod::peekUserMessage(char *messageOut) {
	updateMessageOutBuffer(messageOut);
}


void InputMethod::pushCharacterToMessage(const char c) {
	if(c != CANCEL_CHAR) {
		messageChanged = true;
		messageComplete = false;

		if(userMessageFirstEmptyIndex < MAX_MESSAGE_LENGTH) {
			userMessage[userMessageFirstEmptyIndex] = c;
			userMessageFirstEmptyIndex += 1;

			if(userMessageFirstEmptyIndex == MAX_MESSAGE_LENGTH) {
				DebugLog::getLog().logWarning(MORSE_MESSAGE_TO_SEND_REACHED_MAX_MESSAGE_LENGTH);
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

	if(userMessageFirstEmptyIndex > 0) {
		for(unsigned short i = 0; i < userMessageFirstEmptyIndex; i += 1) {
			if(userMessage[i] > ' ') {
				if(i != 0) {
					//Maybe note this index as the first character index for trimming later
					DebugLog::getLog().logWarning(INPUT_METHOD_MESSAGE_CONTAINS_PRECEDING_WHITESPACE);

					//trim array?
				}
				break;
			}
		}

		for(unsigned short i = userMessageFirstEmptyIndex - 1; i > 0; i -= 1) {
			if(userMessage[i] > ' ') {
				if(i < userMessageFirstEmptyIndex - 1) {
					DebugLog::getLog().logWarning(INPUT_METHOD_MESSAGE_CONTAINS_TRAILING_WHITESPACE); //Maybe change to past-tense: CONTAINED_TRAILING_WHITESPACE
					userMessage[i + 1] = '\0';
				}
				break;
			}
		}
	} else {
		DebugLog::getLog().logError(INPUT_METHOD_COMMIT_EMPTY_MESSAGE);
	}
}


bool InputMethod::isLastCharSpace() const {
	return userMessageFirstEmptyIndex == 0 ? false : (userMessage[userMessageFirstEmptyIndex - 1] == ' ');
}
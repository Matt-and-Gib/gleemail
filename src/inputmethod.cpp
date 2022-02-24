#include "include/inputmethod.h"
#include "include/global.h"


using namespace GLEEMAIL_DEBUG;


InputMethod::InputMethod(void (*c)(char*), void (*s)(char*)) : userMessage{new char[MAX_MESSAGE_LENGTH]}, messageChanged{c}, sendMessage{s} {
	clearUserMessage();
}


InputMethod::~InputMethod() {
	delete[] userMessage;
}


void InputMethod::clearUserMessage() {
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		userMessage[i] = '\0';
	}
	userMessageFirstEmptyIndex = 0;
}


void InputMethod::pushCharacterToMessage(const char& c) {
	if(c != CANCEL_CHAR) {
		if(userMessageFirstEmptyIndex < CHAT_COMPLETE_THRESHOLD) {
			userMessage[userMessageFirstEmptyIndex] = c;
			userMessageFirstEmptyIndex += 1;

			(*messageChanged)(userMessage);

			if(userMessageFirstEmptyIndex == CHAT_COMPLETE_THRESHOLD) {
				DebugLog::getLog().logWarning(MORSE_MESSAGE_TO_SEND_REACHED_CHAT_COMPLETE_THRESHOLD);
				commitMessage();
			}
		} else {
			DebugLog::getLog().logError(MORSE_MESSAGE_TO_SEND_EXCEEDED_CHAT_COMPLETE_THRESHOLD);
			commitMessage();
		}
	} else {
		DebugLog::getLog().logError(MORSE_PUSHED_CHAR_NOTHING);
	}
}


void InputMethod::commitMessage() {
	if(userMessageFirstEmptyIndex > 0) {
		for(unsigned short i = userMessageFirstEmptyIndex - 1; i > 0; i -= 1) {
			if(userMessage[i] == ' ') {
				userMessage[i] = '\0';
			} else {
				break;
			}
		}

		if(userMessage[0] == '\0') {
			DebugLog::getLog().logError(INPUT_METHOD_MESSAGE_ONLY_WHITESPACE);
			return;
		}

		(*sendMessage)(userMessage);
		clearUserMessage();
	} else {
		DebugLog::getLog().logError(INPUT_METHOD_COMMIT_EMPTY_MESSAGE);
	}
}


bool InputMethod::isLastCharSpace() const {
	return userMessageFirstEmptyIndex == 0 ? false : (userMessage[userMessageFirstEmptyIndex - 1] == ' ');
}
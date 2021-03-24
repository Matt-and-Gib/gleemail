#include "inputmethod.h"


void InputMethod::getMessageToSend(char *messageOut) {
	for(int i = 0; i < MAX_MESSAGE_LENGTH; i += 1) {
		messageOut[i] = messageToSend[i];
		messageToSend[i] = '\0';
	}

	messageComplete = false;
}


void InputMethod::pushCharacterToMessage(const char c) {
	messageComplete = false;

	//other logic for appending char to array
}
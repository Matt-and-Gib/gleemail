#ifndef STARTUP_CODE_HANDLER_H
#define STARTUP_CODE_HANDLER_H

template <class T>
class Queue;

template <class U, class V>
class KVPair;

class StartupCodeHandler;


//delete me
#include "Arduino.h"


class StartupCodeHandlerData {
public:
	StartupCodeHandler* const instance;
	bool (StartupCodeHandler::*callback)(void);

	explicit StartupCodeHandlerData(StartupCodeHandler* i, bool (StartupCodeHandler::*c)(void)) : instance{i}, callback{c} {
		if(instance == nullptr) {
			Serial.println(F("just created instance as a nullptr"));
		} else {
			Serial.println(F("instance is all good"));
		}
	}
	~StartupCodeHandlerData() {Serial.println(F("startupciodehandlerdata descrtuctor"));}

};


class StartupCodeHandler {
public:
	virtual void registerNewStartupCodes(Queue<KVPair<char, StartupCodeHandlerData*>>&, StartupCodeHandler* const) = 0;
	virtual void startupCodeReceived(bool (StartupCodeHandler::*)(void)) = 0;

	virtual void test_only_delete_me_asap() = 0;
};

#endif
#ifndef STARTUP_CODE_HANDLER_H
#define STARTUP_CODE_HANDLER_H

template <class T>
class Queue;

template <class U, class V>
class KVPair;

class StartupCodeHandler;


struct StartupCodeHandlerData {
	StartupCodeHandler* instance = nullptr;
	bool (StartupCodeHandler::*callback)(void);

	StartupCodeHandlerData(StartupCodeHandler* i, bool (StartupCodeHandler::*c)(void)) : instance{i}, callback{c} {}
};


class StartupCodeHandler {
private:

public:
	virtual void registerNewStartupCodes(Queue<KVPair<char, StartupCodeHandlerData*>>&) = 0;
	virtual void startupCodeReceived(bool (StartupCodeHandler::*)(void)) = 0;
};

#endif
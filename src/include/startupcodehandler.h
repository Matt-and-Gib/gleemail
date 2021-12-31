#ifndef STARTUP_CODE_HANDLER_H
#define STARTUP_CODE_HANDLER_H

template <class T>
class Queue;

template <class U, class V>
class KVPair;

class StartupCodeHandler;


struct StartupCodeHandlerInfo {
	StartupCodeHandler* instance = nullptr;
	bool (StartupCodeHandler::*callback)(void);

	StartupCodeHandlerInfo(StartupCodeHandler* i, bool (StartupCodeHandler::*c)(void)) : instance{i}, callback{c} {}
};


class StartupCodeHandler {
public:
	virtual void registerNewStartupCodes(Queue<KVPair<char, StartupCodeHandlerInfo*>>&) = 0;
	virtual void startupCodeReceived(bool (StartupCodeHandler::*)(void)) = 0;
};

#endif
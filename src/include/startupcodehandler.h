#ifndef STARTUP_CODE_HANDLER_H
#define STARTUP_CODE_HANDLER_H

template <class T>
class Queue;

template <class U, class V>
class KVPair;

class StartupCodeHandler;


class StartupCodeHandlerData {
public:
	StartupCodeHandler* const instance;
	bool (StartupCodeHandler::*callback)(void);

	explicit StartupCodeHandlerData(StartupCodeHandler* const i, bool (StartupCodeHandler::*c)(void)) : instance{i}, callback{c} {}
};


class StartupCodeHandler {
public:
	virtual void registerNewStartupCodes(Queue<KVPair<const char&, StartupCodeHandlerData* const>>&) = 0;
	virtual void startupCodeReceived(bool (StartupCodeHandler::*)(void)) = 0;
};

#endif
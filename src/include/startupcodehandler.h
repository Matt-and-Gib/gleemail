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
	void (StartupCodeHandler::*callback)(void);

	explicit StartupCodeHandlerData(StartupCodeHandler* const i, void (StartupCodeHandler::*c)(void)) : instance{i}, callback{c} {}
	~StartupCodeHandlerData() = default;
};


class StartupCodeHandler {
public:
	explicit StartupCodeHandler() = default;
	virtual ~StartupCodeHandler() = default;

	virtual void registerNewStartupCodes(Queue<KVPair<const char&, StartupCodeHandlerData* const>>&) = 0;
	virtual void startupCodeReceived(void (StartupCodeHandler::*)(void)) = 0;
};

#endif
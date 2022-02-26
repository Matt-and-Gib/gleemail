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

	StartupCodeHandlerData(StartupCodeHandler* const i, void (StartupCodeHandler::* const c)(void)) : instance{i}, callback{c} {}
	StartupCodeHandlerData(const StartupCodeHandlerData&) = delete;
	StartupCodeHandlerData(StartupCodeHandlerData&&) = delete;
	StartupCodeHandlerData& operator=(const StartupCodeHandlerData&) = delete;
	StartupCodeHandlerData& operator=(StartupCodeHandlerData&&) = delete;
	~StartupCodeHandlerData() = default;
};


class StartupCodeHandler {
public:
	StartupCodeHandler() = default;
	StartupCodeHandler(const StartupCodeHandler&) = delete;
	StartupCodeHandler(StartupCodeHandler&&) = delete;
	StartupCodeHandler& operator=(const StartupCodeHandler&) = delete;
	StartupCodeHandler& operator=(StartupCodeHandler&&) = delete;
	virtual ~StartupCodeHandler() = default;

	virtual void registerNewStartupCodes(Queue<KVPair<const char&, StartupCodeHandlerData* const>>&) = 0;
	virtual void startupCodeReceived(void (StartupCodeHandler::*)(void)) = 0;
};

#endif
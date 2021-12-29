#ifndef STARTUP_CODE_HANDLER_H
#define STARTUP_CODE_HANDLER_H

template <class T>
class Queue;

template <class U, class V>
class KVPair;


class StartupCodeHandler {
private:

public:
	virtual void registerNewStartupCodes(Queue<KVPair<char, bool (StartupCodeHandler::*)(void)>>& startupCodeHandlers) const = 0;
};

#endif
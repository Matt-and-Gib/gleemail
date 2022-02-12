#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H


class WebAccess;
struct Pin;


class InputMethod {
private:
	unsigned long lastDebounceTime = 0;

	//bool messageChanged = false;
	//bool messageComplete = false;
	//bool messageRetrieved = false;
	char *userMessage;
	unsigned short int userMessageFirstEmptyIndex = 0;

	void clearUserMessage();
	//void updateMessageOutBuffer(char *);

	void (*messageChanged)(char*);
	void (*sendMessage)(char*);
public:
	InputMethod(void (*)(char*), void (*)(char*));
	~InputMethod();

	virtual const char* getDataVersionRequestEndpoint() const = 0;
	virtual const char* getDataRequestEndpoint() const = 0;
	virtual bool setNetworkData(const char*) = 0;

	virtual const char* getCachedDataPath() const = 0;

	virtual Pin** getPins() = 0;
	virtual void processInput(const unsigned long) = 0;

	virtual unsigned short getDebounceThreshold() = 0;

	unsigned long getLastDebounceTime() const {return lastDebounceTime;}
	void setLastDebounceTime(const unsigned long t) {lastDebounceTime = t;}

	//void getUserMessage(char *);
	//void peekUserMessage(char *);
	void commitMessage();
	//bool hasMessageChanged() const {return messageChanged;}
	//bool isMessageReady() const {return messageComplete;}
	bool messageNotEmpty() const {return userMessageFirstEmptyIndex > 0;} //userMessage[0] != '\0';}
	bool isLastCharSpace() const;

	void pushCharacterToMessage(const char& c);
};

#endif
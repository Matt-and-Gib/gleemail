#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H


class WebAccess;
struct Pin;


class InputMethod {
private:
	char *userMessage;
	unsigned short int userMessageFirstEmptyIndex = 0;

	void clearUserMessage();

	void (*messageChanged)(char*);
	void (*sendMessage)(char*);
public:
	explicit InputMethod(void (*)(char*), void (*)(char*));
	virtual ~InputMethod();

	virtual const char* getDataVersionRequestEndpoint() const = 0;
	virtual const char* getDataRequestEndpoint() const = 0;
	virtual bool setNetworkData(const char*) = 0;

	virtual const char* getCachedDataPath() const = 0;

	virtual Pin** getPins() = 0;
	virtual void processInput(const unsigned long&) = 0;

	void commitMessage();
	bool messageNotEmpty() const {return userMessageFirstEmptyIndex > 0;}
	bool isLastCharSpace() const;

	void pushCharacterToMessage(const char& c);
};

#endif
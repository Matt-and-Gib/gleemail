#ifndef INPUTMETHOD_H
#define INPUTMETHOD_H


class WebAccess;
struct Pin;


class InputMethod {
private:
	char* userMessage;
	unsigned short int userMessageFirstEmptyIndex = 0;

	void clearUserMessage();

	void (* const messageChanged)(char*);
	void (* const sendMessage)(char*);
public:
	InputMethod(void (* const)(char*), void (* const)(char*));
	InputMethod(const InputMethod&) = delete;
	InputMethod(InputMethod&&) = delete;
	InputMethod& operator=(const InputMethod&) = delete;
	InputMethod& operator=(InputMethod&&) = delete;
	virtual ~InputMethod();

	[[nodiscard]] virtual const char* getDataVersionRequestEndpoint() const = 0;
	[[nodiscard]] virtual const char* getDataRequestEndpoint() const = 0;
	[[nodiscard]] virtual bool setNetworkData(const char*) = 0;

	[[nodiscard]] virtual const char* getCachedDataPath() const = 0;

	[[nodiscard]] virtual Pin** getPins() = 0;
	virtual void processInput(const unsigned long&) = 0;

	void commitMessage();
	[[nodiscard]] bool messageNotEmpty() const {return userMessageFirstEmptyIndex > 0;}
	[[nodiscard]] bool isLastCharSpace() const;

	void pushCharacterToMessage(const char& c);
};

#endif
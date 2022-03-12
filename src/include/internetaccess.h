#ifndef INTERNETACCESS_H
#define INTERNETACCESS_H


class WiFiClient;


class InternetAccess final {
private:
	char* ssid = nullptr;
	char* password = nullptr;
	static const unsigned char MAX_SSID_LENGTH = 32;
	static const unsigned char MAX_PASSWORD_LENGTH = 63; //Note: Serial buffer size is 64

	WiFiClient* client;
public:
	InternetAccess();
	InternetAccess(const InternetAccess&) = delete;
	InternetAccess(InternetAccess&&) = delete;
	InternetAccess& operator=(const InternetAccess&) = delete;
	InternetAccess& operator=(InternetAccess&&) = delete;
	~InternetAccess();

	[[nodiscard]] static unsigned char getMaxSSIDLength() {return MAX_SSID_LENGTH;}
	[[nodiscard]] static unsigned char getMaxPasswordLength() {return MAX_PASSWORD_LENGTH;}

	[[nodiscard]] bool connectToNetwork(const char* networkName, const char* networkPassword, bool retry = true);
	void disconnectFromNetwork();

	[[nodiscard]] bool connectToWeb(const char* address);
	void writeHeaderLine(const char* header);
	[[nodiscard]] bool activeWebConnection() const;
	[[nodiscard]] bool responseAvailableFromWeb();
	[[nodiscard]] char nextCharInWebResponse();
};

#endif
#ifndef INTERNETACCESS_H
#define INTERNETACCESS_H


class WiFiClient;


class InternetAccess {
private:
	char* ssid;
	char* password;
	static const constexpr unsigned char MAX_SSID_LENGTH = 32;
	static const constexpr unsigned char MAX_PASSWORD_LENGTH = 63;

	WiFiClient* client;
public:
	InternetAccess();

	static const unsigned char getMaxSSIDLength() {return MAX_SSID_LENGTH;}
	static const unsigned char getMaxPasswordLength() {return MAX_PASSWORD_LENGTH;}

	bool connectToNetwork(const char* networkName, const char* networkPassword, bool retry = true);
	void disconnectFromNetwork();

	bool connectToWeb(const char* address);
	void writeHeaderLine(const char* header);
	bool activeWebConnection();
	bool responseAvailableFromWeb();
	char nextCharInWebResponse();
};

#endif
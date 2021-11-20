#ifndef INTERNETACCESS_H
#define INTERNETACCESS_H

#include "global.h"

#include <SPI.h>
#include <WiFiNINA.h>


class InternetAccess {
private:
	char* ssid;
	char* password;
	static const constexpr unsigned char MAX_SSID_LENGTH = 32;
	static const constexpr unsigned char MAX_PASSWORD_LENGTH = 63;

	WiFiClient client;
public:
	static const unsigned char getMaxSSIDLength() {return MAX_SSID_LENGTH;}
	static const unsigned char getMaxPasswordLength() {return MAX_PASSWORD_LENGTH;}

	bool connectToNetwork(const char* networkName, const char* networkPassword, bool retry = true);
	void disconnectFromNetwork();

	bool connectToWeb(const char* address) {return client.connectSSL(address, 443);}
	void writeHeaderLine(const char* header) {client.println(header);}
	bool activeWebConnection() {return client.connected();}
	bool responseAvailableFromWeb() {return client.available();}
	char nextCharInWebResponse() {return client.read();}

	IPAddress getLocalIP();
};

#endif
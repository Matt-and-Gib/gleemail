#ifndef INTERNETACCESS_H
#define INTERNETACCESS_H

#include "global.h"

#include <SPI.h>
#include <WiFiNINA.h>


class InternetAccess {
private:
	char* ssid;
	char* password;
	static const constexpr unsigned short MAX_SSID_LENGTH = 32;
	static const constexpr unsigned int MAX_PASSWORD_LENGTH = 63;

	WiFiClient client;
public:
	InternetAccess();
	~InternetAccess();

	unsigned short getMaxSSIDLength() const {return MAX_SSID_LENGTH;}
	unsigned short getMaxPasswordLength() const {return MAX_PASSWORD_LENGTH;}

	bool connectToNetwork(char*, char*, bool);
	void disconnectFromNetwork();

	bool connectToWeb(const char* address) {return client.connectSSL(address, 443);}
	void writeHeaderLine(const char* header) {client.println(header);}
	bool activeWebConnection() {return client.connected();}
	bool responseAvailableFromWeb() {return client.available();}
	char nextCharInWebResponse() {return client.read();}
};


InternetAccess::InternetAccess() {

}


InternetAccess::~InternetAccess() {
	
}


bool InternetAccess::connectToNetwork(char* networkName, char* networkPassword, bool retry = true) {
	if(networkName == nullptr || networkPassword == nullptr) {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_PASSED_INVALID_PARAMETER);
		return false;
	}

	if(WiFi.status() == WL_CONNECTED) {
		disconnectFromNetwork();
	}

	WiFi.begin(networkName, networkPassword);
	while(WiFi.status() == WL_IDLE_STATUS) {
		delay(250);
	}

	if(WiFi.status() == WL_CONNECT_FAILED) {
		DebugLog::getLog().logWarning(ERROR_CODE::NETWORK_CONNECTION_FAILED);
		if(retry) {
			delay(500);
			return connectToNetwork(networkName, networkPassword, false);
		} else {
			DebugLog::getLog().logError(ERROR_CODE::NETWORK_WIFI_CONNECTION_FAILED_RETRY_OCCURRED);
			return false;
		}
	}

	if(WiFi.status() == WL_CONNECTED) {
		return true;
	} else {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_UNKNOWN_STATUS);
		return false;
	}
}


void InternetAccess::disconnectFromNetwork() {
	WiFi.disconnect();
}

#endif
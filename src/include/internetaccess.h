#ifndef INTERNETACCESS_H
#define INTERNETACCESS_H

#ifdef UNIT_TEST_MODE
//include moc_global.h
//include moc_spi.h
//include moc_WiFiNINA.h
#else
#include "global.h"
#include <SPI.h>
#include <WiFiNINA.h>
#endif


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

	bool connectToNetwork(const char*, const char*, bool);
	void disconnectFromNetwork();

	bool connectToWeb(const char* address) {return client.connectSSL(address, 443);}
	void writeHeaderLine(const char* header) {client.println(header);}
	bool activeWebConnection() {return client.connected();}
	bool responseAvailableFromWeb() {return client.available();}
	char nextCharInWebResponse() {return client.read();}

	IPAddress getLocalIP();
};


InternetAccess::InternetAccess() {

}


InternetAccess::~InternetAccess() {
	
}


bool InternetAccess::connectToNetwork(const char* networkName, const char* networkPassword, bool retry = true) {
	if(networkName == nullptr || networkPassword == nullptr) {
		DebugLog::getLog().logError(ERROR_CODE::INTERNET_ACCESS_PASSED_INVALID_PARAMETER);
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
		DebugLog::getLog().logWarning(ERROR_CODE::INTERNET_ACCESS_CONNECTION_FAILED);
		disconnectFromNetwork();
		if(retry) {
			delay(2000);
			return connectToNetwork(networkName, networkPassword, false);
		} else {
			DebugLog::getLog().logError(ERROR_CODE::INTERNET_ACCESS_WIFI_CONNECTION_FAILED_RETRY_OCCURRED);
			return false;
		}
	}

	if(WiFi.status() == WL_DISCONNECTED) {
		DebugLog::getLog().logError(ERROR_CODE::INTERNET_ACCESS_DISCONNECTED_DURING_CONNECTION_ATTEMPT);
		return false;
	}

	const uint8_t tempStat = WiFi.status();
	if(tempStat == WL_CONNECTED) {
		return true;
	} else {
		DebugLog::getLog().logError(ERROR_CODE::INTERNET_ACCESS_UNKNOWN_STATUS);
		Serial.println(tempStat);
		
		return false;
	}
}


void InternetAccess::disconnectFromNetwork() {
	WiFi.disconnect();
}


IPAddress InternetAccess::getLocalIP() {
	if(WiFi.status() != WL_CONNECTED) {
		return nullptr;
	} else {
		return WiFi.localIP();
	}
}


#endif
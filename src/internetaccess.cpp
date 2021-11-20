#include "include/internetaccess.h"


bool InternetAccess::connectToNetwork(const char* networkName, const char* networkPassword, bool retry) {
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
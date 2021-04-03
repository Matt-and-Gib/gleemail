#ifndef NETWORKING_H
#define NETWORKING_H

#include <SPI.h>
#include <WiFiNINA.h>


class Networking {
private:
	bool activeConnection;

	char* ssid;
	char* password;
	static constexpr unsigned short MAX_SSID_LENGTH = 32;
	static constexpr unsigned int MAX_PASSWORD_LENGTH = 63;

	WiFiClient client;

	char* server;

	char* dataBuffer;
	unsigned int dataBufferIndex;

	static constexpr char HEADER_END_STRING[] = "\r\n\r\n";
	static constexpr unsigned short LENGTH_OF_HEADER_END_STRING = sizeof(HEADER_END_STRING)/sizeof(HEADER_END_STRING[0]) - 1;

public:
	Networking();
	~Networking();

	bool connectToNetwork(char*, char*);
	void disconnectFromNetwork();

	char* downloadFromServer(const char*, const char* const*);

	bool checkConnection() const {return activeConnection;}
	unsigned short getMaxSSIDLength() const {return MAX_SSID_LENGTH;}
	unsigned short getMaxPasswordLength() const {return MAX_PASSWORD_LENGTH;}
};


Networking::Networking() {
	activeConnection = false;
}


bool Networking::connectToNetwork(char* networkName, char* networkPassword) {
	if(networkName == nullptr || networkPassword == nullptr) {
		return false;
	}

	if(activeConnection) {
		disconnectFromNetwork();
	}

	unsigned short attemptIndex = 0;
	while(!activeConnection) {
		if(attemptIndex == 4) {
			return false;
		}

		activeConnection = (WiFi.begin(networkName, networkPassword) == WL_CONNECTED) ? true : false;
		attemptIndex += 1;
		delay(3000); //Allow time to for router to assign network properties
	}

	return true;
}


void Networking::disconnectFromNetwork() {
	if(client.connected()) {
		client.stop();
	}
}


char* Networking::downloadFromServer(const char* server, const char* const* headers) {
	return "";
}


#endif
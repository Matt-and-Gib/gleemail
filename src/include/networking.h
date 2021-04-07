#ifndef NETWORKING_H
#define NETWORKING_H

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include "global.h"

//REMOVE ME
//#include "Arduino.h"
//#include "HardwareSerial.h"
//REMOVE ME


class Networking {
private:
	char* ssid;
	char* password;
	static constexpr unsigned short MAX_SSID_LENGTH = 32;
	static constexpr unsigned int MAX_PASSWORD_LENGTH = 63;

	WiFiClient client;
	WiFiUDP Udp;

	char* server;

	static constexpr short DATA_BUFFER_SIZE = 3040; //Buffer Index rouned power of 2 //3035; Buffer index //3072; Suggested size

	static constexpr char HEADER_END_STRING[] = "\r\n\r\n";
	static constexpr unsigned short LENGTH_OF_HEADER_END_STRING = sizeof(HEADER_END_STRING)/sizeof(HEADER_END_STRING[0]) - 1;

	short findEndOfHeaderIndex(const char*, const unsigned short);
public:
	Networking();
	~Networking();

	bool connectToNetwork(char*, char*);
	void disconnectFromNetwork();

	bool connectToPeer(unsigned short (&)[4]);

	bool connectToServer(const char*);
	bool sendRequestToServer(const char* const*);

	char* downloadFromServer(const char*, const char* const*);

	unsigned short getMaxSSIDLength() const {return MAX_SSID_LENGTH;}
	unsigned short getMaxPasswordLength() const {return MAX_PASSWORD_LENGTH;}
};


Networking::Networking() {
	
}

Networking::~Networking() {
	disconnectFromNetwork();
}


bool Networking::connectToNetwork(char* networkName, char* networkPassword) {
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

	if(WiFi.status() == WL_CONNECTED) {
		return true;
	} else {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_CONNECTION_FAILED);
		return false;
	}
}


void Networking::disconnectFromNetwork() {
	WiFi.disconnect();
}


bool Networking::connectToPeer(unsigned short (&ipBlocks)[4]) {
	Udp.begin(CONNECTION_PORT);

	IPAddress friendsIP(ipBlocks[0], ipBlocks[1], ipBlocks[2], ipBlocks[3]);

	Udp.beginPacket(friendsIP, CONNECTION_PORT);
	Udp.write(NETWORK_HANDSHAKE_MESSAGE);
	Udp.endPacket();

	unsigned short packetSize = 0;
	char* receiveBuffer;
	while(true) {
		if(Udp.parsePacket()) {
			packetSize = Udp.read(receiveBuffer, 255);
			receiveBuffer[packetSize] = '\0';
			
			if(receiveBuffer == NETWORK_HANDSHAKE_MESSAGE) { //TODO: implement me! (this is not the correct way to compare c strings)
				return true;
			} else {
				DebugLog::getLog().logError(ERROR_CODE::NETWORK_INVALID_HANDSHAKE_MESSAGE);
				return false;
			}
		}

		delay(1000);
	}
}


bool Networking::connectToServer(const char* address) {
	if(!client.connectSSL(address, 443)) {
		return false;
	}

	return true;
}


bool Networking::sendRequestToServer(const char* const* headers) {
	const char* headerLine = headers[0];
	if(headerLine == nullptr) {
		return false;
	}

	int headerIndex = 0;
	while(headerLine != nullptr) {
		if(headerIndex > 16) { //Replace magic number
			DebugLog::getLog().logError(ERROR_CODE::NETWORK_HEADER_TERMINATION_OMITTED);
			return false;
		}

		client.println(headerLine);
		headerLine = headers[++headerIndex];
	}

	return true;
}


short Networking::findEndOfHeaderIndex(const char* const rawData, const unsigned short lengthOfData) {
	unsigned short headerEndSearchIndex = 0;
	short endOfHeaderIndex = -1;

	for(unsigned short beginningSubstringIndex = 0; beginningSubstringIndex < lengthOfData; beginningSubstringIndex += 1) {
		if(rawData[beginningSubstringIndex] == '\r') {
			headerEndSearchIndex = 0;
			for(unsigned short endSubstringIndex = 0; endSubstringIndex < LENGTH_OF_HEADER_TERMINATION; endSubstringIndex += 1) {
				if(rawData[beginningSubstringIndex + endSubstringIndex] != HEADER_TERMINATION[headerEndSearchIndex++]) {
					goto headerEndMismatch;
				}
			}

			endOfHeaderIndex = beginningSubstringIndex + LENGTH_OF_HEADER_TERMINATION;
			break;
		}
		headerEndMismatch:;
	}

	return endOfHeaderIndex;
}


char* Networking::downloadFromServer(const char* server, const char* const* headers) {
	if(WiFi.status() != WL_CONNECTED) {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_DOWNLOAD_IMPOSSIBLE_NOT_CONNECTED);
		return nullptr;
	}

	if(!connectToServer(server)) {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_SECURE_CONNECTION_TO_SERVER_FAILED);
		return nullptr;
	}

	if(!sendRequestToServer(headers)) {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_REQUEST_TO_SERVER_HEADER_INVALID);
		return nullptr;
	}

	int bufferIndex = 0;
	char* dataBuffer = new char[DATA_BUFFER_SIZE];
	while(client.connected()) {
		while(client.available()) {
			if(bufferIndex < DATA_BUFFER_SIZE) {
				dataBuffer[bufferIndex++] = client.read();
			} else {
				DebugLog::getLog().logError(ERROR_CODE::NETWORK_DATA_BUFFER_OVERFLOW);
				return nullptr;
			}
		}
	}

	if(bufferIndex < DATA_BUFFER_SIZE/2) {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_DATA_BUFFER_UNDERUTILIZED, false);
	}

	/*//Print full response
	for(int i = 0; i < bufferIndex; i += 1) {
		Serial.print(dataBuffer[i]);
	}*/

	/*//Print buffer utilization
	Serial.print("Used ");
	Serial.print(bufferIndex);
	Serial.print(" out of max ");
	Serial.println(DATA_BUFFER_SIZE);*/

	short endOfHeaderIndex = findEndOfHeaderIndex(dataBuffer, bufferIndex);
	if(endOfHeaderIndex != -1) {
		const unsigned short LENGTH_OF_JSON_BODY = bufferIndex - endOfHeaderIndex;
		char* jsonData = new char[LENGTH_OF_JSON_BODY];
		for(int i = 0; i < LENGTH_OF_JSON_BODY; i += 1) {
			jsonData[i] = dataBuffer[endOfHeaderIndex + i];
		}
		jsonData[LENGTH_OF_JSON_BODY] = '\0';

		delete[] dataBuffer;
		return jsonData;
	}

	return nullptr;
}


#endif
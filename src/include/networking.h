#ifndef NETWORKING_H
#define NETWORKING_H

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include <ArduinoJson.h>

#include "global.h"

//REMOVE ME
#include "Arduino.h"
#include "HardwareSerial.h"
//REMOVE ME


class Networking {
private:
	char* ssid;
	char* password;
	static const constexpr unsigned short MAX_SSID_LENGTH = 32;
	static const constexpr unsigned int MAX_PASSWORD_LENGTH = 63;

	WiFiClient client;
	WiFiUDP udp;
	IPAddress peerIPAddress;

	char* server;
	unsigned short packetSize = 0;

	static const constexpr short DATA_BUFFER_SIZE = 3040; //Buffer Index rouned power of 2 //3035; Buffer index //3072; Suggested size

	static const constexpr char HEADER_END_STRING[] = "\r\n\r\n";
	static const constexpr unsigned short LENGTH_OF_HEADER_END_STRING = sizeof(HEADER_END_STRING)/sizeof(HEADER_END_STRING[0]) - 1;

	short findEndOfHeaderIndex(const char*, const unsigned short);

	char* createMessage(char*, const MESSAGE_TYPE);
public:
	Networking();
	~Networking();

	bool connectToNetwork(char*, char*, bool);
	void disconnectFromNetwork();

	bool messageAvailable();
	bool readMessage(char*, const unsigned short);
	bool writeMessage(char*, const MESSAGE_TYPE);

	bool connectToPeer(IPAddress&);

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


bool Networking::connectToNetwork(char* networkName, char* networkPassword, bool retry = true) {
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


void Networking::disconnectFromNetwork() {
	WiFi.disconnect();
}


bool Networking::messageAvailable() {
	return udp.parsePacket();
}


bool Networking::readMessage(char* buffer, const unsigned short bufferLength) {
	packetSize = udp.read(buffer, bufferLength);
	buffer[packetSize] = '\0';
	return true;
}


char* Networking::createMessage(char* body, const MESSAGE_TYPE messageType) {
	unsigned int DOC_SIZE = 4096;
	char* messagePayload = new char[DOC_SIZE]; //REMEMBER TO DELETE ME!

	DynamicJsonDocument payload(DOC_SIZE);

	switch(messageType) {
	case MESSAGE_TYPE::HANDSHAKE:
		payload["header"]["type"] = "handshake";
		payload["body"]["message"] = "four score and seven years ago our forefathers declared this land to be free from British oversight";
	break;

	case MESSAGE_TYPE::CHAT:
		Serial.println("got here");
		payload["header"]["type"] = "chat";
		payload["body"]["message"] = body;
	break;

	case MESSAGE_TYPE::LIFELINE:
	break;

	default:
	break;
	}

	serializeJson(payload, messagePayload, measureJson(payload));

	return messagePayload;
}


bool Networking::writeMessage(char* buffer, const MESSAGE_TYPE messageType) {
	if(peerIPAddress) {
		udp.beginPacket(peerIPAddress, CONNECTION_PORT);
		udp.write(*createMessage(buffer, messageType));
		udp.endPacket();
		return true;
	} else {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_INVALID_PEER_IP_ADDRESS);
		return false;
	}
}


bool Networking::connectToPeer(IPAddress& connectToIP) {
	udp.begin(CONNECTION_PORT);

	//Change to use WriteMessage
	udp.beginPacket(connectToIP, CONNECTION_PORT);
	udp.write(NETWORK_HANDSHAKE_CHARACTER);
	udp.endPacket();

	char* receiveBuffer = new char[2];
	while(true) {
		if(udp.parsePacket()) {
			packetSize = udp.read(receiveBuffer, 2);
			receiveBuffer[packetSize] = '\0';

			if(receiveBuffer[0] == NETWORK_HANDSHAKE_CHARACTER) {
				if(udp.remoteIP() != connectToIP) {
					DebugLog::getLog().logWarning(ERROR_CODE::NETWORK_UNEXPECTED_HANDSHAKE_IP);
					return false;
				} else {
					peerIPAddress = connectToIP;
					//Change to use WriteMessage
					udp.beginPacket(peerIPAddress, CONNECTION_PORT);
					udp.write(NETWORK_HANDSHAKE_CHARACTER);
					udp.endPacket();
					return true;
				}
			} else {
				DebugLog::getLog().logError(ERROR_CODE::NETWORK_INVALID_HANDSHAKE_MESSAGE);
				return false;
			}
		}

		delay(1000);
	}

	delete[] receiveBuffer;
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
		DebugLog::getLog().logWarning(ERROR_CODE::NETWORK_DATA_BUFFER_UNDERUTILIZED);
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
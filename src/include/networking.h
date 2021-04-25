#ifndef NETWORKING_H
#define NETWORKING_H

#include "Arduino.h" //Must include for timing e.g. millis()
#include <WiFiUdp.h>
#include <ArduinoJson.hpp>

#include "global.h"
#include "queue.h"

#include "internetaccess.h"
#include "security.h"


class Message {
private:
	const char* body;
	MESSAGE_TYPE messageType;
public:
	Message() {}
	Message(const DynamicJsonDocument&);
	~Message() {}

	char* toString();

	bool operator==(const Message& o) {return messageType == o.messageType;}
	bool operator==(const MESSAGE_TYPE& o) {return messageType == o;}
};


Message::Message(const DynamicJsonDocument& payload) {
	const int messageT = payload["header"]["type"];
	messageType = static_cast<MESSAGE_TYPE>(messageT);
	body = payload["body"]["message"];
}


class Networking {
private:
	WiFiUDP udp;
	IPAddress peerIPAddress;

	static const constexpr unsigned short MESSAGE_BUFFER_SIZE = 4096;
	char* messageBuffer = new char[MESSAGE_BUFFER_SIZE];
	unsigned short messageSize = 0;

	//Queue<Message> messagePool;
	//void clearMessageBuffer();
	//char* createMessage(char*, const MESSAGE_TYPE);

	unsigned long long processStartTime = 0;
	short processRunTime = 0;

	short getMessages(const unsigned short);
	short getMessagesRemainingTime = 0;
	static const constexpr unsigned short MAX_GET_MESSAGES_PROCESS_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	short processIncomingMessages(const unsigned short);
	short processIncomingMessagesRemainingTime = 0;
	static const constexpr unsigned short MAX_PROCESS_INCOMING_MESSAGES_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	short sendOutgoingMessages(const unsigned short);
	short sendOutgoingMessagesRemainingTime = 0;
	static const constexpr unsigned short MAX_SEND_OUTGOING_MESSAGES_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;
public:
	Networking();
	~Networking();

	void processNetwork(const unsigned long long);

	bool connectToPeer(IPAddress&) {return false;} //FINISH OR REMOVE ME!
};


Networking::Networking() {
	for(int i = 0; i < MESSAGE_BUFFER_SIZE + 1; i += 1) {
		messageBuffer[i] = '\0';
	}
}

Networking::~Networking() {
	delete[] messageBuffer;
}



short Networking::getMessages(const unsigned short processingTimeOffset = 0) {
	processStartTime = millis();

	while(millis() - processStartTime < MAX_GET_MESSAGES_PROCESS_DURATION_MS - processingTimeOffset) {
		if(udp.parsePacket()) {
			messageSize = udp.read(messageBuffer, MESSAGE_BUFFER_SIZE);
			if(udp.remoteIP() == peerIPAddress) {
				//decrypt message
				//parse into json
				//construct message object
				//enqueue
			} else {
				//debug message: unknown sender
			}
		} else {
			break;
		}
	}

	processRunTime = MAX_GET_MESSAGES_PROCESS_DURATION_MS - (millis() - processStartTime);
	if(processRunTime < 0) {
		//debug message: took too long
	}

	return processRunTime;
}


short Networking::processIncomingMessages(const unsigned short processingTimeOffset = 0) {
	processStartTime = millis();

	while(millis() - processStartTime < MAX_PROCESS_INCOMING_MESSAGES_DURATION_MS - processingTimeOffset) {

	}

	processRunTime = MAX_PROCESS_INCOMING_MESSAGES_DURATION_MS - (millis() - processStartTime);
	if(processRunTime < 0) {
		//debug message
	}

	return processRunTime;
}


short Networking::sendOutgoingMessages(const unsigned short processingTimeOffset = 0) {
	processStartTime = millis();

	while(millis() - processStartTime < MAX_SEND_OUTGOING_MESSAGES_DURATION_MS - processingTimeOffset) {

	}

	processRunTime = MAX_SEND_OUTGOING_MESSAGES_DURATION_MS - (millis() - processStartTime);
	if(processRunTime < 0) {
		//debug message
	}

	return processRunTime;
}


void Networking::processNetwork(const unsigned long long cycleStartTime) {
	getMessagesRemainingTime = getMessages();
	processIncomingMessagesRemainingTime = processIncomingMessages(getMessagesRemainingTime);
	sendOutgoingMessagesRemainingTime = sendOutgoingMessages(processIncomingMessagesRemainingTime);
}


/*bool Networking::messageAvailable() {
	return udp.parsePacket();
}*/


/*void Networking::clearMessageBuffer() {
	for(int i = 0; i < MESSAGE_BUFFER_SIZE; i += 1) {
		if(messageBuffer[i] == '\0') {
			break;
		}

		messageBuffer[i] = '\0';
	}
}*/


/*bool Networking::readMessage(char* buffer, const unsigned short bufferLength) {
	clearMessageBuffer();
	packetSize = udp.read(messageBuffer, MESSAGE_BUFFER_SIZE);

	if(packetSize > bufferLength) {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_PACKET_SIZE_GREATER_THAN_BUFFER_LENGTH);
	}

	ArduinoJson::DynamicJsonDocument doc(MESSAGE_BUFFER_SIZE); //REMEMBER: this will not be enough if the message takes up its full size
	ArduinoJson::DeserializationError error = deserializeJson(doc, messageBuffer);

	if(error) {
		//Serial.print("JSON Error: ");
		//Serial.print(error.f_str());
		DebugLog::getLog().logError(JSON_DESERIALIZATION_ERROR);
		return false;
	} else {
		//const char* messageBody = doc["body"]["message"];
		//for(int i = 0; i < bufferLength; i += 1) {
			//buffer[i] = messageBody[i];
		//}

		return true;
	}
}*/


/*Message* Networking::createMessage(char* body, const MESSAGE_TYPE messageType) {
	clearMessageBuffer();
	DynamicJsonDocument payload(MESSAGE_BUFFER_SIZE);

	switch(messageType) {
	case MESSAGE_TYPE::HANDSHAKE:
		payload["header"]["type"] = static_cast<short>(MESSAGE_TYPE::HANDSHAKE);
		payload["body"]["message"] = "";
	break;

	case MESSAGE_TYPE::CHAT:
		payload["header"]["type"] = static_cast<short>(MESSAGE_TYPE::CHAT);
		payload["body"]["message"] = body;
	break;

	default:
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_UNKNOWN_MESSAGE_TYPE);
	break;
	}

	serializeJson(payload, messageBuffer, measureJson(payload) + 1);
	return nullptr;
}*/


/*bool Networking::writeMessage(char* buffer, const MESSAGE_TYPE messageType = MESSAGE_TYPE::CHAT) {
	if(peerIPAddress) {
		udp.beginPacket(peerIPAddress, CONNECTION_PORT);
		udp.write(createMessage(buffer, messageType).serializeToString());
		udp.endPacket();
		return true;
	} else {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_INVALID_PEER_IP_ADDRESS);
		return false;
	}
}*/


/*bool Networking::connectToPeer(IPAddress& connectToIP) {
	udp.begin(CONNECTION_PORT);

	//Change to use WriteMessage
	//udp.beginPacket(connectToIP, CONNECTION_PORT);
	//udp.write(createMessage(nullptr, MESSAGE_TYPE::HANDSHAKE));//NETWORK_HANDSHAKE_CHARACTER);
	//udp.endPacket();

	peerIPAddress = connectToIP;
	writeMessage(nullptr, MESSAGE_TYPE::HANDSHAKE);

	char* receiveBuffer = new char[2];*/
	/*while(true) {
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
	}*/
/*
	//DELETE ME
	return true;

	delete[] receiveBuffer;
}*/


#endif
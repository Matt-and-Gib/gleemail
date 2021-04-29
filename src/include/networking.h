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
	unsigned short packetSize = 0;
	//unsigned short messageSize = 0;

	//Queue<Message> messagePool;
	//void clearMessageBuffer();
	//char* createMessage(char*, const MESSAGE_TYPE);

	unsigned long long processStartTime = 0;
	short processRunTime = 0;

	short timeSensitiveProcessDuration = 0;

	bool getMessages();
	static const constexpr unsigned short MAX_GET_MESSAGES_PROCESS_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	bool processIncomingMessages();
	static const constexpr unsigned short MAX_PROCESS_INCOMING_MESSAGES_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	bool sendOutgoingMessages();
	static const constexpr unsigned short MAX_SEND_OUTGOING_MESSAGES_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	short doTimeSensesitiveProcess(const unsigned short, bool (Networking::*)(), const unsigned short);
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



bool Networking::getMessages() {
	packetSize = udp.parsePacket();
	if(packetSize > 0) {
		udp.read(messageBuffer, packetSize);
		if(udp.remoteIP() == peerIPAddress) {
			//decrypt message
			//parse into json
			//construct message object
			//enqueue
		} else {
			//debug message: unknown sender
		}

		return true;
	} else {
		return false;
	}
}


/*
void Networking::processIncomingMessageByType(const Message& msg) {
	messageReceivedCount += 1;
	messageHash = hashMessage(msg);
	if(messageHash != msg.hash) {
		return;
	}

	switch(msg.type) {
	case MESSAGETYPE::ERROR:
		DebugLog::getLog().logError();
	break;

	case MESSAGETYPE::HEARTBEAT:
		lastHeartbeatMS = millis();
	break;

	case MESSAGETYPE::CONFIRMATION:
		Message* confirmedMessage = messagesOutHead.removeByIdempotencyToken(msg.idempotencyToken);
		if(confirmedMessage != nullptr) {
			confirmedMessage->callback();
			delete confirmatedMessage;
		}
	break;

	case MESSAGETYPE::CHAT:
		messagesOutHead.enqueue(new Message(MESSAGETYPE::CONFIRMATION, IdempotencyToken::generate()));
		if(messagesInHead.uniqueIdempotencyToken(msg.idempotencyToken)) {
			messagesInIdempotencyTokens.enqueue(msg.idempotencyToken);
			receivedMessage = msg.body;
		}
	break;

	case MESSAGETYPE::HANDSHAKE:
		messagesOutHead.removeByType(MESSAGETYPE::HANDSHAKE);
		messagesOutHead.enqueue(new Message(MESSAGETYPE::CONFIRMATION, IdempotencyToken::generate()));
		doConnected();
		handshakeReceivedCount += 1;

		if(handshareReceivedCount > MAX_HANDSHAKE_THRESHOLD) {
			DebugLog::getLog().logError();
		}
	break;

	default:
	break;
	}
}
*/


bool Networking::processIncomingMessages() {
	//if messageIn queue contains messages
	//peek next message
		//if message type equals current message type
			//process messageIn
		//step forward in queue
		//if currentMessage == nullptr
			//increment message type priority
			//if message type priority == DONE
				//return false;
		//return true;
	//else return false;
}


bool Networking::sendOutgoingMessages() {
	//if messageOut queue contains messages
		//if message type equals current message type
			//if messageResendDelay elapsed
				//serialize message
				//encrypt message
				//send message
				//do message type callback
		//step forward in queue
		//if current message == nullptr
			//increment message type priority
			//if message type priority == DONE
				//return false
		//return true
	//else return false
}


short Networking::doTimeSensesitiveProcess(const unsigned short processingTimeOffset, bool (Networking::*doProcess)(), const unsigned short MAX_PROCESSING_TIME) {
	processStartTime = millis();

	while(millis() - processStartTime < MAX_PROCESSING_TIME - processingTimeOffset) {
		if(!(this->*doProcess)()) {
			break;
		}
	}

	processRunTime = MAX_PROCESSING_TIME - (millis() - processStartTime);
	if(processRunTime < 0) {
		//debug message
	}

	return processRunTime;
}


void Networking::processNetwork(const unsigned long long cycleStartTime) {
	timeSensitiveProcessDuration = 0;

	timeSensitiveProcessDuration = doTimeSensesitiveProcess(timeSensitiveProcessDuration, &Networking::getMessages, MAX_GET_MESSAGES_PROCESS_DURATION_MS);
	timeSensitiveProcessDuration = doTimeSensesitiveProcess(timeSensitiveProcessDuration, &Networking::processIncomingMessages, MAX_PROCESS_INCOMING_MESSAGES_DURATION_MS);
	if(timeSensitiveProcessDuration < 0) {
		//check message count threshold, drop connection/blacklist ip if exceeded
	}

	timeSensitiveProcessDuration = doTimeSensesitiveProcess(timeSensitiveProcessDuration, &Networking::sendOutgoingMessages, MAX_SEND_OUTGOING_MESSAGES_DURATION_MS);
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
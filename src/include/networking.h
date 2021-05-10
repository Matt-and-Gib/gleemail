#ifndef NETWORKING_H
#define NETWORKING_H

#include "HardwareSerial.h"

#include <WiFiUdp.h>
#include <ArduinoJson.hpp>

#include "global.h"
#include "queue.h"

#include "internetaccess.h"
#include "security.h"


class IdempotencyToken {
private:
	unsigned short value;
	unsigned long timestamp;
	unsigned short retryCount;
public:
	IdempotencyToken() {
		value = 0;
		timestamp = 0;
		retryCount = 0;
	}
	IdempotencyToken(const unsigned short v, const unsigned long t) {
		value = v;
		timestamp = t;
		retryCount = 0;
	}
	IdempotencyToken(const IdempotencyToken& i) {
		value = i.getValue();
		timestamp = i.getTimestamp();
		retryCount = i.getRetryCount();
	}
	~IdempotencyToken() {}

	const unsigned short getValue() const {return value;}
	const unsigned long getTimestamp() const {return timestamp;}
	const unsigned short getRetryCount() const {return retryCount;}
	void incrementRetryCount() {retryCount += 1;}
};


class MessageError {
private:
	ERROR_CODE id;
	const char* attribute;
public:
	MessageError();
	MessageError(const StaticJsonDocument<JSON_DOCUMENT_SIZE>&);
	~MessageError();

	const ERROR_CODE getID() const {return id;}
	const char* getAttribute() const {return attribute;}
};


MessageError::MessageError() {

}


MessageError::MessageError(const StaticJsonDocument<JSON_DOCUMENT_SIZE>& parsedDocument) {
	const unsigned short tempErrorID = parsedDocument["E"]["D"];
	id = static_cast<ERROR_CODE>(tempErrorID);

	attribute = parsedDocument["E"]["A"];

	//DebugLog::getLog().logWarning(id);
}


MessageError::~MessageError() {
	delete attribute;
}


class Message {
private:
	MESSAGE_TYPE messageType;
	IdempotencyToken* idempotencyToken;
	const char* chat;
	MessageError* error;
public:
	Message();
	Message(const StaticJsonDocument<JSON_DOCUMENT_SIZE>&, const unsigned long);
	Message(MESSAGE_TYPE, IdempotencyToken*, char*, MessageError*);
	~Message();

	const MESSAGE_TYPE getMessageType() const {return messageType;}
	IdempotencyToken* getIdempotencyToken() {return idempotencyToken;}
	const char* getChat() {return chat;}
	MessageError* getError() {return error;}
	char* toString();

	bool operator==(const Message& o) {return messageType == o.messageType;}
	bool operator==(const MESSAGE_TYPE& o) {return messageType == o;}
};


Message::Message() {

}



Message::Message(const StaticJsonDocument<JSON_DOCUMENT_SIZE>& parsedDocument, const unsigned long currentTimeMS) {
	const unsigned short tempMessageType = parsedDocument["T"];
	Serial.print("Constructing message with type: ");
	Serial.println(tempMessageType);
	messageType = static_cast<MESSAGE_TYPE>(tempMessageType);

	unsigned short tempIdempVal = parsedDocument["I"];
	idempotencyToken = new IdempotencyToken(tempIdempVal, currentTimeMS);
	chat = parsedDocument["C"];
	error = new MessageError(parsedDocument);
}


Message::Message(MESSAGE_TYPE t, IdempotencyToken* i, char* c, MessageError* e) {
	messageType = t;
	idempotencyToken = i;
	chat = c;
	error = e;
}


Message::~Message() {
	delete[] idempotencyToken;
	delete chat;
	delete[] error;
}


class Networking {
private:
	WiFiUDP udp;
	IPAddress peerIPAddress;
	//unsigned long lastHeartbeatMS = 0;
	//static const constexpr unsigned short FLATLINE_THRESHOLD_MS = 5000;
	static const constexpr unsigned short MAX_OUTGOING_MESSAGE_RETRY_COUNT = 10;
	static const constexpr unsigned short RESEND_OUTGOING_MESSAGE_THRESHOLD_MS = 6000;

	unsigned long uuid;
	unsigned short messagesSentCount = 0;

	const unsigned long (*nowMS)();
	//void checkHeartbeat();
	unsigned long approxCurrentTime;

	Message* heartbeat;
	unsigned long lastHeartbeatSentMS = 0;
	unsigned long lastHeartbeatReceivedMS = 0;
	static const constexpr unsigned short HEARTBEAT_RESEND_THRESHOLD_MS = 3000;
	static const constexpr unsigned short FLATLINE_THRESHOLD_MS = 5 * HEARTBEAT_RESEND_THRESHOLD_MS;
	void sendHeartbeat();
	void processHeartbeat(const unsigned long);
	void checkHeartbeats();

	static const constexpr unsigned short MESSAGE_BUFFER_SIZE = 4096;
	char* messageBuffer = new char[MESSAGE_BUFFER_SIZE];
	unsigned short packetSize = 0;

	Queue<Message> messagesIn = *new Queue<Message>();
	Queue<IdempotencyToken> messagesInIdempotencyTokens = *new Queue<IdempotencyToken>;
	Queue<Message> messagesOut = *new Queue<Message>();
	MESSAGE_TYPE searchMessageType;

	unsigned long long processStartTime = 0;
	short processElapsedTime = 0;

	bool getMessages();
	static const constexpr unsigned short MAX_GET_MESSAGES_PROCESS_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	bool processIncomingMessagesQueue();
	static const constexpr unsigned short MAX_PROCESS_INCOMING_MESSAGES_QUEUE_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	bool processOutgoingMessagesQueue();
	static const constexpr unsigned short MAX_SEND_OUTGOING_MESSAGES_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;
	void sendOutgoingMessage(Message&);

	bool doTimeSensesitiveProcess(const unsigned short, bool (Networking::*)(), const unsigned short);
	void processIncomingMessage(QueueNode<Message>&);
	unsigned short messageReceivedCount = 0;
	static const constexpr unsigned short MAX_MESSAGE_RECEIVED_COUNT = 10;

	bool exceededMaxOutgoingTokenRetryCount();
	void removeExpiredIncomingIdempotencyTokens();
public:
	Networking(const unsigned long (*)(), const long u);
	~Networking();

	void processNetwork();

	bool connectToPeer(IPAddress&); //{return false;} //FINISH OR REMOVE ME!
};


Networking::Networking(const unsigned long (*millis)(), const long u) {
	nowMS = millis;
	uuid = u + nowMS(); //CHANGE ME!
	heartbeat = new Message(MESSAGE_TYPE::HEARTBEAT, new IdempotencyToken(uuid + messagesSentCount, nowMS()), nullptr, nullptr);

	for(int i = 0; i < MESSAGE_BUFFER_SIZE + 1; i += 1) {
		messageBuffer[i] = '\0';
	}
}

Networking::~Networking() {
	delete[] messageBuffer;
}


//REWRITE ME
bool Networking::connectToPeer(IPAddress& connectToIP) {
	udp.begin(CONNECTION_PORT);

	//Change to use WriteMessage
	//udp.beginPacket(connectToIP, CONNECTION_PORT);
	//udp.write(createMessage(nullptr, MESSAGE_TYPE::HANDSHAKE));//NETWORK_HANDSHAKE_CHARACTER);
	//udp.endPacket();

	peerIPAddress = connectToIP;
	//writeMessage(nullptr, MESSAGE_TYPE::HANDSHAKE);
	udp.beginPacket(peerIPAddress, CONNECTION_PORT);
	udp.write('$');
	udp.endPacket();

	char* receiveBuffer = new char[2];
	while(true) {
		if(udp.parsePacket()) {
			packetSize = udp.read(receiveBuffer, 2);
			receiveBuffer[packetSize] = '\0';

			peerIPAddress = connectToIP;
			//Change to use WriteMessage
			udp.beginPacket(peerIPAddress, CONNECTION_PORT);
			udp.write('$');
			udp.endPacket();
			return true;
		}

		delay(1000);
	}

	//DELETE ME
	return true;

	delete[] receiveBuffer;
}
//REWRITE ME


void Networking::checkHeartbeats() {
	approxCurrentTime = nowMS();

	if(approxCurrentTime - lastHeartbeatSentMS > HEARTBEAT_RESEND_THRESHOLD_MS) {
		sendHeartbeat();
		lastHeartbeatSentMS = approxCurrentTime;
	}

	if(approxCurrentTime - lastHeartbeatReceivedMS > FLATLINE_THRESHOLD_MS) {
		DebugLog::getLog().logError(NETWORK_HEARTBEAT_FLATLINE);
		//cut ourselves off from gleepal :<
	}
}


void Networking::sendHeartbeat() {
	Serial.print("Sending heartbeat... ");
	sendOutgoingMessage(*heartbeat);
	Serial.println("Done!");
}


bool Networking::doTimeSensesitiveProcess(const unsigned short previousProcessElapsedTime, bool (Networking::*doProcess)(), const unsigned short MAX_PROCESSING_TIME) {
	processStartTime = nowMS();
	while(nowMS() - processStartTime < MAX_PROCESSING_TIME + (MAX_PROCESSING_TIME - previousProcessElapsedTime)) {
		if(!(this->*doProcess)()) {
			break;
		}
	}

	processElapsedTime = nowMS() - processStartTime; 
	if(processElapsedTime > MAX_PROCESSING_TIME) {
		if(processElapsedTime > 2 * MAX_PROCESSING_TIME) {
			DebugLog::getLog().logError(NETWORK_TIME_SENSITIVE_PROCESS_EXCEEDED_ALLOCATED_TIME_SIGNIFICANT);
		} else {
			DebugLog::getLog().logWarning(NETWORK_TIME_SENSITIVE_PROCESS_EXCEEDED_ALLOCATED_TIME_INSIGNFICANT);
		}
		return false;
	} else {
		return true;
	}
}


bool Networking::getMessages() {
	packetSize = udp.parsePacket();
	if(packetSize > 0) {
		udp.read(messageBuffer, packetSize);
		if(udp.remoteIP() == peerIPAddress) {
			//decrypt message

			messageReceivedCount += 1;

			StaticJsonDocument<JSON_DOCUMENT_SIZE> parsedDocument;
			DeserializationError parsingError = deserializeJson(parsedDocument, messageBuffer, JSON_DOCUMENT_SIZE);
			if(parsingError) {
				Serial.println("Parse failed");
				DebugLog::getLog().logError(JSON_MESSAGE_DESERIALIZATION_ERROR);
				return true;
			}

			Serial.println("Parse succeeded. Enqueueing message");

			messagesIn.enqueue(new Message(parsedDocument, nowMS()));
		} else {
			DebugLog::getLog().logWarning(NETWORK_UNKNOWN_MESSAGE_SENDER);
		}

		return true;
	} else {
		return false;
	}
}


bool Networking::processIncomingMessagesQueue() {
	QueueNode<Message>* nextMessage = messagesIn.peek();
	if(nextMessage == nullptr) {
		return false;
	}

	do {
		if(nextMessage->getData()->getMessageType() == searchMessageType) {
			messagesIn.remove(*nextMessage);
			processIncomingMessage(*nextMessage);
			delete nextMessage;
			return true;
		}

		nextMessage = nextMessage->getNode();
	} while(nextMessage != nullptr);

	searchMessageType = static_cast<MESSAGE_TYPE>(static_cast<short>(searchMessageType) + 1);
	if(searchMessageType == MESSAGE_TYPE::NONE) {
		return false;
	} else {
		return true;
	}
}


void Networking::processIncomingMessage(QueueNode<Message>& msg) {
	switch(msg.getData()->getMessageType()) {
	case MESSAGE_TYPE::ERROR:
		//check for unique idempotency token
		messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));
		//DebugLog::getLog().logWarning();
	break;

	case MESSAGE_TYPE::HEARTBEAT:
		processHeartbeat(nowMS());
	break;

	case MESSAGE_TYPE::CONFIRMATION:
		/*Message* confirmedMessage = messagesOutHead.removeByIdempotencyToken(msg.idempotencyToken);
		if(confirmedMessage != nullptr) {
			confirmedMessage->callback();
			delete confirmatedMessage;
		}*/
	break;

	case MESSAGE_TYPE::CHAT:
		//check for unique idempotency token
		messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));
		/*messagesOutHead.enqueue(new Message(MESSAGETYPE::CONFIRMATION, IdempotencyToken::generate()));
		if(messagesInHead.uniqueIdempotencyToken(msg.idempotencyToken)) {
			messagesInIdempotencyTokens.enqueue(msg.idempotencyToken);
			receivedMessage = msg.body;
		}*/
	break;

	case MESSAGE_TYPE::HANDSHAKE:
		//check for unique idempotency token
		messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));
		/*messagesOutHead.removeByType(MESSAGETYPE::HANDSHAKE);
		messagesOutHead.enqueue(new Message(MESSAGETYPE::CONFIRMATION, IdempotencyToken::generate()));
		doConnected();
		handshakeReceivedCount += 1;

		if(handshareReceivedCount > MAX_HANDSHAKE_THRESHOLD) {
			DebugLog::getLog().logError();
		}*/
	break;

	default:
		DebugLog::getLog().logError(NETWORK_UNKNOWN_INCOMING_MESSAGE_TYPE);
	break;
	}
}


void Networking::processHeartbeat(const unsigned long time) {
	lastHeartbeatReceivedMS = time;
	Serial.println("Received heartbeat!");
}


bool Networking::processOutgoingMessagesQueue() {
	QueueNode<Message>* nextMessage = messagesOut.peek();
	if(nextMessage == nullptr) {
		return false;
	}

	do {
		if(nextMessage->getData()->getMessageType() == searchMessageType) {
			if(nowMS() > nextMessage->getData()->getIdempotencyToken()->getRetryCount() + (nextMessage->getData()->getIdempotencyToken()->getRetryCount() * RESEND_OUTGOING_MESSAGE_THRESHOLD_MS)) {
				sendOutgoingMessage(*(nextMessage->getData()));
				//do callback?
				return true;
			}
		}

		nextMessage = nextMessage->getNode();
	} while (nextMessage != nullptr);

	searchMessageType = static_cast<MESSAGE_TYPE>(static_cast<short>(searchMessageType) + 1);
	if(searchMessageType == MESSAGE_TYPE::NONE) {
		return false;
	} else {
		return true;
	}
}


void Networking::sendOutgoingMessage(Message& msg) {
	char outputBuffer[JSON_DOCUMENT_SIZE];
	StaticJsonDocument<JSON_DOCUMENT_SIZE> doc;

	doc["T"] = static_cast<short>(msg.getMessageType());
	doc["I"] = msg.getIdempotencyToken()->getValue();
	doc["C"] = msg.getChat();

	JsonObject E = doc.createNestedObject("E");
	E["D"] = static_cast<unsigned short>(msg.getError()->getID());
	E["A"] = msg.getError()->getAttribute();

	serializeJson(doc, outputBuffer);
	//encryptBuffer(outputBuffer, measureJson(doc) + 1);

	udp.beginPacket(peerIPAddress, CONNECTION_PORT);
	udp.write(outputBuffer);
	udp.endPacket();

	msg.getIdempotencyToken()->incrementRetryCount();
}


bool Networking::exceededMaxOutgoingTokenRetryCount() {
	//find first non-handshake message
	if(messagesOut.peek()->getData()->getIdempotencyToken()->getRetryCount() > MAX_OUTGOING_MESSAGE_RETRY_COUNT) {
		return true;
	} else {
		return false;
	}
}


// Potential optimization: process more than one token per frame & make into time sensitive process
void Networking::removeExpiredIncomingIdempotencyTokens() {
	IdempotencyToken* nextToken = messagesInIdempotencyTokens.peek()->getData();
	if(nextToken == nullptr) {
		return;
	}

	if(nowMS() > nextToken->getTimestamp() + (MAX_OUTGOING_MESSAGE_RETRY_COUNT * RESEND_OUTGOING_MESSAGE_THRESHOLD_MS)) {
		messagesInIdempotencyTokens.dequeue();
		delete nextToken;
	}
}


void Networking::processNetwork() {
	Serial.println("top");
	checkHeartbeats();

	Serial.println("Get messages");
	if(!doTimeSensesitiveProcess(MAX_GET_MESSAGES_PROCESS_DURATION_MS, &Networking::getMessages, MAX_GET_MESSAGES_PROCESS_DURATION_MS)) {
		if(messageReceivedCount > MAX_MESSAGE_RECEIVED_COUNT) {
			DebugLog::getLog().logError(NETWORK_TOO_MANY_MESSAGES_RECEIVED);
			//drop connection
		}
	} else {
		messageReceivedCount = 0;
	}

	Serial.println("Process incoming messages queue");
	searchMessageType = static_cast<MESSAGE_TYPE>(0);
	doTimeSensesitiveProcess(processElapsedTime, &Networking::processIncomingMessagesQueue, MAX_GET_MESSAGES_PROCESS_DURATION_MS);

	Serial.println("Process outgoing messages queue");
	searchMessageType = static_cast<MESSAGE_TYPE>(0);
	if(!doTimeSensesitiveProcess(processElapsedTime, &Networking::processOutgoingMessagesQueue, MAX_SEND_OUTGOING_MESSAGES_DURATION_MS)) {
		if(exceededMaxOutgoingTokenRetryCount()) {
			DebugLog::getLog().logError(NETWORK_OUTGOING_TOKEN_TIMESTAMP_ELAPSED);
			//drop connection
		}
	}

	removeExpiredIncomingIdempotencyTokens();
	Serial.println("bottom");
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

	//DELETE ME
	return true;

	delete[] receiveBuffer;
}*/


#endif
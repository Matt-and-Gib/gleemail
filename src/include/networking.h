#ifndef NETWORKING_H
#define NETWORKING_H

#include <WiFiUdp.h>
#include <ArduinoJson.hpp>

#include "global.h"
#include "queue.h"


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
	~IdempotencyToken() {
		Serial.print("destructor for token ");
		Serial.println(value);
	}

	bool operator==(const IdempotencyToken& o) {return value == o.getValue();}

	const unsigned short getValue() const {return value;}
	const unsigned long getTimestamp() const {return timestamp;}
	const unsigned short getRetryCount() const {return retryCount;}
	void incrementRetryCount() {retryCount += 1;}
};


/*class MessageError {
private:
	ERROR_CODE id;
	const char* attribute;
public:
	MessageError() {
		id = ERROR_CODE::MESSAGE_ERROR_NONE;
		attribute = new char[0];
	}
	MessageError(const StaticJsonDocument<JSON_DOCUMENT_SIZE>& parsedDocument) {
		const unsigned short tempErrorID = parsedDocument["E"]["D"];
		id = static_cast<ERROR_CODE>(tempErrorID);
		attribute = parsedDocument["E"]["A"];
	}
	~MessageError() {
		delete[] attribute;
	}

	const ERROR_CODE getID() const {return id;}
	const char* getAttribute() const {return attribute;}
};*/


class Message {
private:
	MESSAGE_TYPE messageType;
	IdempotencyToken* idempotencyToken;
	const char* chat;
	//MessageError* error;

	static bool noProcess(Queue<Message>& q, QueueNode<Message>& n) {return true;}
	bool (*postProcess)(Queue<Message>&, QueueNode<Message>&);
public:
	Message() {

	}
	Message(const StaticJsonDocument<JSON_DOCUMENT_SIZE>& parsedDocument, const unsigned long currentTimeMS) {
		const unsigned short tempMessageType = parsedDocument["T"];
		messageType = static_cast<MESSAGE_TYPE>(tempMessageType);

		unsigned short tempIdempVal = parsedDocument["I"];
		idempotencyToken = new IdempotencyToken(tempIdempVal, currentTimeMS);
		const char* tempChat = parsedDocument["C"];
		chat = copyString(tempChat, MAX_MESSAGE_LENGTH);
		//error = new MessageError(parsedDocument);

		postProcess = &noProcess;
	}
	Message(MESSAGE_TYPE t, IdempotencyToken* i, char* c, /*MessageError* e,*/ bool (*p)(Queue<Message>&, QueueNode<Message>&) = &noProcess) {
		messageType = t;
		idempotencyToken = i;
		chat = c;
		//error = e;
		postProcess = p;
	}
	~Message() {
		delete idempotencyToken;
		delete[] chat;
		//delete error;
	}

	bool operator==(Message& o) {return (*idempotencyToken == *o.getIdempotencyToken());}

	const MESSAGE_TYPE getMessageType() const {return messageType;}
	IdempotencyToken* getIdempotencyToken() {return idempotencyToken;}
	const char* getChat() {return chat;}
	//MessageError* getError() {return error;}
	bool doPostProcess(Queue<Message>& q, QueueNode<Message>& n) {return (*postProcess)(q, n);}
};


class Networking {
private:
	WiFiUDP udp;
	IPAddress peerIPAddress;

	static const constexpr unsigned short MAX_OUTGOING_MESSAGE_RETRY_COUNT = 10;
	static const constexpr unsigned short RESEND_OUTGOING_MESSAGE_THRESHOLD_MS = 500; //minimize in the future
	static const constexpr unsigned short INCOMING_IDEMPOTENCY_TOKEN_EXPIRED_THRESHOLD_MS = (MAX_OUTGOING_MESSAGE_RETRY_COUNT * RESEND_OUTGOING_MESSAGE_THRESHOLD_MS) + RESEND_OUTGOING_MESSAGE_THRESHOLD_MS;

	unsigned long uuid;
	unsigned short messagesSentCount = 0;

	const unsigned long (*nowMS)();
	unsigned long approxCurrentTime;

	Message* heartbeat;
	unsigned long lastHeartbeatSentMS = 0;
	unsigned long lastHeartbeatReceivedMS = 0;
	static const constexpr unsigned short HEARTBEAT_RESEND_THRESHOLD_MS = 1000;
	static const constexpr unsigned short FLATLINE_THRESHOLD_MS = 4 * HEARTBEAT_RESEND_THRESHOLD_MS;
	void sendHeartbeat();
	void processHeartbeat(const unsigned long);
	void checkHeartbeats();

	char* messageBuffer = new char[JSON_DOCUMENT_SIZE];
	unsigned short packetSize = 0;

	Queue<Message> messagesIn;
	Queue<IdempotencyToken> messagesInIdempotencyTokens;
	Queue<Message> messagesOut;
	MESSAGE_TYPE searchMessageType;

	unsigned long long processStartTime = 0;
	short processElapsedTime = 0;

	bool doTimeSensesitiveProcess(const unsigned short, const unsigned short, bool (Networking::*)(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&), bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);

	QueueNode<Message>* queueStartNode;
	QueueNode<Message>* holdingNode;
	QueueNode<Message>* messageOutWithMatchingIdempotencyToken;
	bool processQueue(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);
	bool processIncomingMessageQueueNode(Queue<Message>&, QueueNode<Message>*);
	static const constexpr unsigned short MAX_PROCESS_INCOMING_MESSAGE_QUEUE_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;
	
	void (*chatMessageReceivedCallback)(const char*);

	bool processOutgoingMessageQueueNode(Queue<Message>&, QueueNode<Message>*);
	static const constexpr unsigned short MAX_PROCESS_OUTGOING_MESSAGE_QUEUE_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	bool getMessages(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);
	static const constexpr unsigned short MAX_GET_MESSAGES_PROCESS_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;
	unsigned short messageReceivedCount = 0;
	static const constexpr unsigned short MAX_MESSAGE_RECEIVED_COUNT = 10;

	void sendOutgoingMessage(Message&);

	void processIncomingMessage(QueueNode<Message>&);

	bool exceededMaxOutgoingTokenRetryCount();
	void removeExpiredIncomingIdempotencyToken();

	static bool removeFromQueue(Queue<Message>& fromQueue, QueueNode<Message>& node) {
		delete fromQueue.remove(node);
		return true;
	}
public:
	Networking(const unsigned long (*)(), void (*)(const char*), const long u);
	~Networking();

	void processNetwork();
	void sendChatMessage(const char*);

	bool connectToPeer(IPAddress&); //FINISH OR REMOVE ME!
};


Networking::Networking(const unsigned long (*millis)(), void (*chatMsgCallback)(const char*), const long u) {
	nowMS = millis;
	uuid = u + nowMS(); //CHANGE ME!
	chatMessageReceivedCallback = chatMsgCallback;

	heartbeat = new Message(MESSAGE_TYPE::HEARTBEAT, new IdempotencyToken(0, 0), /*nullptr,*/ nullptr);

	for(int i = 0; i < JSON_DOCUMENT_SIZE; i += 1) {
		messageBuffer[i] = '\0';
	}
}


Networking::~Networking() {
	delete[] messageBuffer;
}


void Networking::sendChatMessage(const char* chat) {
	messagesOut.enqueue(new Message(MESSAGE_TYPE::CHAT, new IdempotencyToken(uuid + messagesSentCount, nowMS()), copyString(chat, MAX_MESSAGE_LENGTH)/*, nullptr*/));
	Serial.print("enqueued chat message with body: ");
	Serial.println(chat);
}


//REWRITE ME
bool Networking::connectToPeer(IPAddress& connectToIP) {
	udp.begin(CONNECTION_PORT);

	peerIPAddress = connectToIP;

	udp.beginPacket(peerIPAddress, CONNECTION_PORT);
	udp.write('$');
	udp.endPacket();
	sendHeartbeat();

	char* receiveBuffer = new char[2];
	while(true) {
		if(udp.parsePacket()) {
			packetSize = udp.read(receiveBuffer, 2);
			receiveBuffer[packetSize] = '\0';

			peerIPAddress = connectToIP;

			udp.beginPacket(peerIPAddress, CONNECTION_PORT);
			udp.write('$');
			udp.endPacket();
			sendHeartbeat();

			return true;
		}

		delay(1000);
	}

	return true;

	delete[] receiveBuffer;
}
//REWRITE ME


// Potential optimization: process more than one token per frame & make into time sensitive process
void Networking::removeExpiredIncomingIdempotencyToken() {
	QueueNode<IdempotencyToken>* nextTokenNode = messagesInIdempotencyTokens.peek();
	if(nextTokenNode == nullptr) {
		return;
	}

	if(nowMS() > nextTokenNode->getData()->getTimestamp() + INCOMING_IDEMPOTENCY_TOKEN_EXPIRED_THRESHOLD_MS) {
		Serial.println("Incoming token idempotency token expired!");
		messagesInIdempotencyTokens.dequeue();
		delete nextTokenNode;
		Serial.println("deleted");
	}
}


bool Networking::exceededMaxOutgoingTokenRetryCount() {
	//find first non-handshake message
	if(messagesOut.peek()->getData()->getIdempotencyToken()->getRetryCount() >= MAX_OUTGOING_MESSAGE_RETRY_COUNT) {
		return true;
	} else {
		return false;
	}
}


void Networking::sendOutgoingMessage(Message& msg) {
	char outputBuffer[JSON_DOCUMENT_SIZE];
	StaticJsonDocument<JSON_DOCUMENT_SIZE> doc;

	doc["T"] = static_cast<unsigned short>(msg.getMessageType());
	doc["I"] = msg.getIdempotencyToken()->getValue();
	doc["C"] = msg.getChat();

	/*JsonObject E = doc.createNestedObject("E");
	E["D"] = static_cast<unsigned short>(msg.getError()->getID());
	E["A"] = msg.getError()->getAttribute();*/

	serializeJson(doc, outputBuffer);
	//encryptBuffer(outputBuffer, measureJson(doc) + 1);

	Serial.print("Sending: ");
	Serial.println(outputBuffer);

	udp.beginPacket(peerIPAddress, CONNECTION_PORT);
	udp.write(outputBuffer);
	udp.endPacket();

	msg.getIdempotencyToken()->incrementRetryCount();
	messagesSentCount += 1;
}


bool Networking::processOutgoingMessageQueueNode(Queue<Message>& messagesOut, QueueNode<Message>* nextMessage) {
	if(nowMS() > nextMessage->getData()->getIdempotencyToken()->getTimestamp() + (nextMessage->getData()->getIdempotencyToken()->getRetryCount() * RESEND_OUTGOING_MESSAGE_THRESHOLD_MS)) {
		sendOutgoingMessage(*(nextMessage->getData()));

		//do callback? (delete confirmation message?)
		nextMessage->getData()->doPostProcess(messagesOut, *nextMessage);
		return true;
	} else {
		return false;
	}
}


void Networking::sendHeartbeat() {
	sendOutgoingMessage(*heartbeat);
}


void Networking::checkHeartbeats() {
	approxCurrentTime = nowMS();

	if(approxCurrentTime - lastHeartbeatReceivedMS > FLATLINE_THRESHOLD_MS) {
		DebugLog::getLog().logError(NETWORK_HEARTBEAT_FLATLINE);
		//cut ourselves off from gleepal :<
		return;
	}

	if(approxCurrentTime - lastHeartbeatSentMS > HEARTBEAT_RESEND_THRESHOLD_MS) {
		sendHeartbeat();
		lastHeartbeatSentMS = approxCurrentTime;
	}
}


void Networking::processHeartbeat(const unsigned long time) {
	lastHeartbeatReceivedMS = time;
}


void Networking::processIncomingMessage(QueueNode<Message>& msg) {
	switch(msg.getData()->getMessageType()) {
	case MESSAGE_TYPE::ERROR:
		//check for unique idempotency token
		messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));
		//DebugLog::getLog().logWarning(msg.getData()->getError()->getID());
	break;

	case MESSAGE_TYPE::HEARTBEAT:
		processHeartbeat(nowMS());
	break;

	case MESSAGE_TYPE::CONFIRMATION:
		messageOutWithMatchingIdempotencyToken = messagesOut.find(*msg.getData());
		if(messageOutWithMatchingIdempotencyToken) {
			Serial.println("found chat to match confirmation");
			messagesOut.remove(*messageOutWithMatchingIdempotencyToken); //memory leak
		} else {
			DebugLog::getLog().logWarning(NETWORK_CONFIRMATION_NO_MATCH_FOUND);
		}
	
	
		/*Message* confirmedMessage = messagesOutHead.removeByIdempotencyToken(msg.idempotencyToken);
		if(confirmedMessage != nullptr) {
			confirmedMessage->callback();
			delete confirmatedMessage;
		}*/
	break;

	//NOTE: ProcessIncomingMessageQueueNode will call Display function if message type is CHAT, adding ~1ms processing time
	case MESSAGE_TYPE::CHAT:
		messagesOut.enqueue(new Message(MESSAGE_TYPE::CONFIRMATION, new IdempotencyToken(msg.getData()->getIdempotencyToken()->getValue(), nowMS()), nullptr, /*nullptr,*/ &removeFromQueue));

		if(!messagesInIdempotencyTokens.find(*(msg.getData()->getIdempotencyToken()))) {
			messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken()))); //Maybe this copy constructor is the source of a seg fault on delete?
			(*chatMessageReceivedCallback)(msg.getData()->getChat());
		}
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
		DebugLog::getLog().logWarning(NETWORK_UNKNOWN_INCOMING_MESSAGE_TYPE);
	break;
	}
}


bool Networking::processIncomingMessageQueueNode(Queue<Message>& messagesIn, QueueNode<Message>* nextMessage) {
	messagesIn.remove(*nextMessage);
	processIncomingMessage(*nextMessage);
	Serial.println("deleting incoming message queue node");
	delete nextMessage;
	Serial.println("deleted incoming message queue node");
	return true;
}


bool Networking::processQueue(bool (Networking::*processMessage)(Queue<Message>&, QueueNode<Message>*), Queue<Message>& fromQueue) {
	while(queueStartNode) {
		if(queueStartNode->getData()->getMessageType() == searchMessageType) {
			holdingNode = queueStartNode->getNode();
			if((this->*processMessage)(fromQueue, queueStartNode)) {
				Serial.println("process queue succeeded");
				queueStartNode = holdingNode;
				return true;
			}
		}

		queueStartNode = queueStartNode->getNode();
	}

	queueStartNode = fromQueue.peek();
	searchMessageType = static_cast<MESSAGE_TYPE>(static_cast<short>(searchMessageType) + 1);
	if(searchMessageType == MESSAGE_TYPE::NONE) {
		return false;
	} else {
		return true;
	}
}


bool Networking::getMessages(bool (Networking::*callback)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&intoQueue) {
	packetSize = udp.parsePacket();
	if(packetSize > 0) {
		udp.read(messageBuffer, packetSize);
		if(udp.remoteIP() == peerIPAddress) {
			//decrypt message

			messageReceivedCount += 1;

			StaticJsonDocument<JSON_DOCUMENT_SIZE> parsedDocument;
			DeserializationError parsingError = deserializeJson(parsedDocument, messageBuffer, JSON_DOCUMENT_SIZE);
			if(parsingError) {
				DebugLog::getLog().logError(JSON_MESSAGE_DESERIALIZATION_ERROR);
				return true;
			}

			//Serial.print("Got: ");
			//Serial.println(messageBuffer);

			intoQueue.enqueue(new Message(parsedDocument, nowMS()));
			/*if(!((this->*callback)(intoQueue, intoQueue.enqueue(new Message(parsedDocument, nowMS()))))) {
				//log error?
			}*/
		} else {
			DebugLog::getLog().logWarning(NETWORK_UNKNOWN_MESSAGE_SENDER);
		}

		return true;
	} else {
		return false;
	}
}


bool Networking::doTimeSensesitiveProcess(const unsigned short previousProcessElapsedTime, const unsigned short MAX_PROCESSING_TIME, bool (Networking::*doProcess)(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&), bool (Networking::*passProcess)(Queue<Message>&, QueueNode<Message>*), Queue<Message>& onQueue) {
	processStartTime = nowMS();
	while(nowMS() - processStartTime < MAX_PROCESSING_TIME + (MAX_PROCESSING_TIME - previousProcessElapsedTime)) {
		if(!(this->*doProcess)(passProcess, onQueue)) {
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


void Networking::processNetwork() {
	//Serial.print('.');
	if(!doTimeSensesitiveProcess(MAX_GET_MESSAGES_PROCESS_DURATION_MS, MAX_GET_MESSAGES_PROCESS_DURATION_MS, &Networking::getMessages, nullptr, messagesIn)) {
		//Maybe log error about get messages (specifically) being slow

		if(messageReceivedCount > MAX_MESSAGE_RECEIVED_COUNT) {
			DebugLog::getLog().logError(NETWORK_TOO_MANY_MESSAGES_RECEIVED);
			//drop connection
		}
	} else {
		messageReceivedCount = 0;
	}

	//NOTE: ProcessIncomingMessageQueueNode will call Display function if message type is CHAT, adding ~1ms processing time
	queueStartNode = messagesIn.peek();
	if(queueStartNode) {
		searchMessageType = START_MESSAGE_TYPE;
		if(!doTimeSensesitiveProcess(processElapsedTime, MAX_PROCESS_INCOMING_MESSAGE_QUEUE_DURATION_MS, &Networking::processQueue, &Networking::processIncomingMessageQueueNode, messagesIn)) {
			//Maybe log error about process incoming messages (specifically) being slow
		}
	}

	checkHeartbeats();

	queueStartNode = messagesOut.peek();
	if(queueStartNode) {
		searchMessageType = START_MESSAGE_TYPE;
		if(!doTimeSensesitiveProcess(processElapsedTime, MAX_PROCESS_OUTGOING_MESSAGE_QUEUE_DURATION_MS, &Networking::processQueue, &Networking::processOutgoingMessageQueueNode, messagesOut)) {
			//Maybe log error about process outgoing messages (specifically) being slow
			if(exceededMaxOutgoingTokenRetryCount()) {
				DebugLog::getLog().logError(NETWORK_OUTGOING_TOKEN_TIMESTAMP_ELAPSED);
				//drop connection
			}
		}

		/*********************************************************	DEBUG ONLY	*/
		/*if(exceededMaxOutgoingTokenRetryCount()) {
			Serial.println("outgoing message exceeded max count... removing!");
			delete messagesOut.dequeue();

			Serial.println("adding idempotency token for testing purposes...");
			messagesInIdempotencyTokens.enqueue(new IdempotencyToken(617, nowMS()));
			Serial.println("...enqueued!");
		}*/
		/*********************************************************	DEBUG ONLY	*/
	}

	removeExpiredIncomingIdempotencyToken();
}

#endif
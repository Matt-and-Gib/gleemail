#ifndef NETWORKING_H
#define NETWORKING_H

#include <WiFiUdp.h>
#include <ArduinoJson.hpp>

#include "global.h"
#include "queue.h"

#include "lib/LiteChaCha/keyinfrastructure.h"
#include "lib/LiteChaCha/authenticatedencrypt.h"


// To use in the future to map IP addresses (or uuids)
class glEEpal {
private:
	IPAddress palIPAddress;
	unsigned short outgoingHandshakeIdempotencyTokenValue;
public:
	glEEpal() {}

	glEEpal(const IPAddress ip, const unsigned short h) {
		palIPAddress = ip;
		outgoingHandshakeIdempotencyTokenValue = h;
	}

	bool operator==(glEEpal& o) {
		return palIPAddress == o.getIPAddress();
	}
	bool operator==(IPAddress a) {
		return palIPAddress == a;
	}

	const IPAddress getIPAddress() const {return palIPAddress;}
	const unsigned short getHandshakeIdempotencyTokenValue() const {return outgoingHandshakeIdempotencyTokenValue;}
};
glEEpal* glEEself = new glEEpal();


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

	}

	bool operator==(const IdempotencyToken& o) {return value == o.getValue();}

	const unsigned short getValue() const {return value;}
	const unsigned long getTimestamp() const {return timestamp;}
	const unsigned short getRetryCount() const {return retryCount;}
	void incrementRetryCount() {retryCount += 1;}
};


//MessageError will cause seg faults when deleted if dynamic memory has not been allocated (You can only delete what yo have new'd!) ☜(ﾟヮﾟ☜)
/*class MessageError {
private:
	ERROR_CODE id;
	const char* attribute;

	static const constexpr unsigned short MAX_ATTRIBUTE_SIZE = 1;
public:
	MessageError() {
		id = ERROR_CODE::MESSAGE_ERROR_NONE;
		attribute = new char[MAX_ATTRIBUTE_SIZE];
		//attribute[0] = '\0';
	}
	MessageError(const ERROR_CODE c, const char* a) {
		id = c;
		attribute = a;
	}
	MessageError(const StaticJsonDocument<JSON_DOCUMENT_SIZE>& parsedDocument) {
		const unsigned short tempErrorID = parsedDocument["E"]["D"];
		id = static_cast<ERROR_CODE>(tempErrorID);

		const char* tempAttribute = parsedDocument["E"]["A"];
		attribute = copyString(tempAttribute, MAX_ATTRIBUTE_SIZE);
	}
	~MessageError() {
		delete[] attribute;
	}

	const ERROR_CODE getID() const {return id;}
	const char* getAttribute() const {return attribute;}
};*/


class Networking;


class Message {
private:
	glEEpal& sender = *glEEself;

	MESSAGE_TYPE messageType;
	IdempotencyToken* idempotencyToken;
	char* chat; //MAKE ME CONSTANT AGAIN!
	//MessageError* error;

	static void noOutgoingProcess(Queue<Message>& q, Message& n) {}
	void (*outgoingPostProcess)(Queue<Message>&, Message&); //Used for removing confirmation messages once sent

	static void noConfirmedProcess(Networking& n, Queue<Message>& messagesOut, QueueNode<Message>& messageIn, Message& messageOut) {}
	void (*confirmedPostProcess)(Networking&, Queue<Message>&, QueueNode<Message>&, Message&); //Used for establishing connection and 
public:
	Message() {}
	Message(const StaticJsonDocument<JSON_DOCUMENT_SIZE>& parsedDocument, const unsigned long currentTimeMS, glEEpal& from) {
		const unsigned short tempMessageType = parsedDocument["T"];
		messageType = static_cast<MESSAGE_TYPE>(tempMessageType);

		unsigned short tempIdempVal = parsedDocument["I"];
		idempotencyToken = new IdempotencyToken(tempIdempVal, currentTimeMS);
		const char* tempChat = parsedDocument["C"];
		chat = copyString(tempChat, MAX_MESSAGE_LENGTH);
		//error = new MessageError(parsedDocument);

		outgoingPostProcess = &noOutgoingProcess;
		confirmedPostProcess = &noConfirmedProcess;

		sender = from;
	}
	Message(MESSAGE_TYPE t, IdempotencyToken* i, char* c, /*MessageError* e,*/ void (*op)(Queue<Message>&, Message&), void (*cp)(Networking&, Queue<Message>&, QueueNode<Message>&, Message&)) {
		messageType = t;
		idempotencyToken = i;
		chat = c;
		//error = e;
		outgoingPostProcess = !op ? &noOutgoingProcess : op; //Remove conditional check by relocating noOutgoingProcess?
		confirmedPostProcess = !cp ? &noConfirmedProcess : cp; //Remove conditional check by relocating noIncomingProcess?
	}
	~Message() {
		delete idempotencyToken;
		delete[] chat;
		//delete error;
	}

	bool operator==(Message& o) {return (*idempotencyToken == *o.getIdempotencyToken());}

	const glEEpal& getSender() const {return sender;}
	const MESSAGE_TYPE getMessageType() const {return messageType;}
	IdempotencyToken* getIdempotencyToken() {return idempotencyToken;}
	char* getChat() {return chat;} //MAKE ME CONSTANT AGAIN!
	//MessageError* getError() {return error;}
	void doOutgoingPostProcess(Queue<Message>& q, Message& n) {return (*outgoingPostProcess)(q, n);}
	void doConfirmedPostProcess(Networking& n, Queue<Message>& mo, QueueNode<Message>& messageIn) {(*confirmedPostProcess)(n, mo, messageIn, *this);}
};


class Networking {
private:
	WiFiUDP udp;
	glEEpal* glEEpalInfo;

	bool connected = false;

	static const constexpr unsigned short MAX_OUTGOING_MESSAGE_RETRY_COUNT = 10;
	static const constexpr unsigned short RESEND_OUTGOING_MESSAGE_THRESHOLD_MS = 500;
	static const constexpr unsigned short INCOMING_IDEMPOTENCY_TOKEN_EXPIRED_THRESHOLD_MS = (MAX_OUTGOING_MESSAGE_RETRY_COUNT * RESEND_OUTGOING_MESSAGE_THRESHOLD_MS) + RESEND_OUTGOING_MESSAGE_THRESHOLD_MS;
	static const constexpr unsigned short SIZE_OF_ENCRYPTION_INFO_PAYLOAD = 265; //32: DSAPubKey + 32: EphemeralPubKey + 64: signature + 4: ID + 1: nullterminator //Don't forget to move me if the rest of encryption is moved

	unsigned long uuid;
	unsigned short messagesSentCount = 0;

	const unsigned long (*nowMS)();
	unsigned long approxCurrentTime;

	CipherManagement ae;
	static const constexpr size_t tagBytes = 16; //CipherManagement::getTagBytes();
	unsigned long long messageCount; // Used to increment a nonce for each new message sent. Will be sent with each encrypted message.
	char tag[tagBytes]; // Used to authenticate encrypted messages. Will be sent with each encrypted message.
	//char* tag;


//	MOVE ME!
	KeyManagement pki; // What is this doing here, I mean, really?

	static const constexpr size_t keyBytes = 32; //pki.getKeyBytes();
	static const constexpr size_t signatureBytes = 64; //pki.getSignatureBytes();
	static const constexpr size_t IDBytes = 4; //pki.getIDBytes();

	char userDSAPrivateKey[keyBytes]; // Used as either an input or an output depending on whether the user would like to generate a new key pair.
	char userDSAPubKey[keyBytes]; // Used as either an input or an output depending on whether the user would like to generate a new key pair.
	char peerDSAPubKey[keyBytes];

	char userEphemeralPubKey[keyBytes];
	char peerEphemeralPubKey[keyBytes];

	char userSignature[signatureBytes];
	char peerSignature[signatureBytes];

	char userID[IDBytes]; // IDs are used for a fixed portion of a nonce.
	char peerID[IDBytes];

	char encryptionInfo[SIZE_OF_ENCRYPTION_INFO_PAYLOAD];
//	MOVE ME!


	void createEncryptionInfoPayload(char*, const char*, const char*, const char*, const char*); // REMOVE ME?
	void stringToHex(char*, char*, const unsigned short, const unsigned short); // REMOVE ME?
	void convertEncryptionInfoPayload(char*, char*, char*, char*, char*); // REMOVE ME?

	void clearAllQueues();
	void dropConnection();

	Message* heartbeat;
	unsigned long lastHeartbeatSentMS = 0;
	unsigned long lastHeartbeatReceivedMS = 0;
	static const constexpr unsigned short HEARTBEAT_RESEND_THRESHOLD_MS = 1000;
	static const constexpr unsigned short FLATLINE_THRESHOLD_MS = 4 * HEARTBEAT_RESEND_THRESHOLD_MS;
	void sendHeartbeat();
	void listenToHeartbeat(const unsigned long);
	void checkHeartbeat();
	void dontCheckHeartbeat() {}
	void (Networking::*processHeartbeat)() = &Networking::dontCheckHeartbeat; //Switch to checkHeartbeat() on successful connection

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

	//These two functions are use from doConfirmedPostProcess()
	static void removeFromQueue(Networking& n, Queue<Message>& messagesOutQueue, QueueNode<Message>& messageIn, Message& messageOut) {
		removeFromQueue(messagesOutQueue, messageOut);
	}
	static void removeFromQueue(Queue<Message>& fromQueue, Message& node) {
		delete fromQueue.remove(node);
	}

	static void connectionEstablished(Networking& n, Queue<Message>& messagesOutQueue, QueueNode<Message>& messageIn, Message& messageOut) {
		Serial.println(F("beginning auth"));
		Serial.flush();

		n.convertEncryptionInfoPayload(n.peerDSAPubKey, n.peerEphemeralPubKey, n.peerSignature, n.peerID, messageIn.getData()->getChat());

		if((n.pki.IDUnique(n.userID, n.peerID)) && (n.pki.signatureValid(n.peerDSAPubKey, n.peerEphemeralPubKey, n.peerSignature))) {
			n.pki.createSessionKey(n.peerEphemeralPubKey); // Creates a shared private session key, overwriting peerEphemeralPubKey, if both users have different IDs and the peer's signature is valid.
			Serial.println(F("authenticated!"));
		} else {
			Serial.println(F("auth failed!"));
		}

		n.ae.initialize(n.peerEphemeralPubKey, n.userID, n.peerID);
		//Free to delete pki, keyBytes, signatureBytes, IDBytes, userDSAPrivateKey, userDSAPublicKey, peerDSAPublicKey, userEphemeralPubKey, peerEphemeralPubKey, userSignature, peerSignature, userID, peerID, encryptionInfo

		delete messagesOutQueue.remove(messageOut); //removes outgoing handshake from queue
		Serial.println(F("Connected to peer!"));

		n.connected = true;
		n.processHeartbeat = &Networking::checkHeartbeat; //MOVE THIS SOMEWHERE ELSE! Maybe set processHeartbeat here to a new function pointer pointing to a "waiting for first heartbeat" function which will (when called) set the processHeartbeat variable to checkHeartbeat.
		n.listenToHeartbeat(n.nowMS());
	}
public:
	Networking(const unsigned long (*)(), void (*)(const char*), const long u);
	~Networking();

	void processNetwork();
	void sendChatMessage(const char*);

	bool connectToPeer(IPAddress&);
};


Networking::Networking(const unsigned long (*millis)(), void (*chatMsgCallback)(const char*), const long u) {
	nowMS = millis;
	uuid = u + nowMS(); //CHANGE ME!
	chatMessageReceivedCallback = chatMsgCallback;

	heartbeat = new Message(MESSAGE_TYPE::HEARTBEAT, new IdempotencyToken(0, 0), nullptr, /*nullptr,*/ nullptr, nullptr);

	for(int i = 0; i < JSON_DOCUMENT_SIZE; i += 1) {
		messageBuffer[i] = '\0';
	}

	//tag = new char[tagBytes];
}


Networking::~Networking() {
	delete glEEpalInfo;
	delete[] messageBuffer;
}


void Networking::clearAllQueues() {
	delete messagesIn.peek();
	delete messagesInIdempotencyTokens.peek();
	delete messagesOut.peek();

	queueStartNode = nullptr;
	holdingNode = nullptr;
	messageOutWithMatchingIdempotencyToken = nullptr;
}


//Connection is being dropped due to heartbeat flatline before other party has finished processing authentication
void Networking::dropConnection() { //Baby, come back (to finish me)
/*	connected = false;
	processHeartbeat = &Networking::dontCheckHeartbeat;

	IPAddress palIP = glEEpalInfo->getIPAddress();
	delete glEEpalInfo;
	glEEpalInfo = nullptr;

	clearAllQueues();

	connectToPeer(palIP);*/

	Serial.println(F("Drop Connection!!!!!!!"));

	//abort();
}


void Networking::sendChatMessage(const char* chat) {
	messagesOut.enqueue(new Message(MESSAGE_TYPE::CHAT, new IdempotencyToken(uuid + messagesSentCount, nowMS()), copyString(chat, MAX_MESSAGE_LENGTH)/*, nullptr*/, nullptr, &removeFromQueue));
}


void Networking::createEncryptionInfoPayload(char* encryptionInfoOut, const char* DSAPubKey, const char* ephemeralPubKey, const char* signature, const char* ID) {
	unsigned short i = 0;
	for(i = 0; i < keyBytes; i += 1) {
		encryptionInfoOut[i*2] = DSAPubKey[i] >> 4;
		encryptionInfoOut[(i*2) + 1] = DSAPubKey[i] & 0x0f;

		encryptionInfoOut[(i*2) + (keyBytes*2)] = ephemeralPubKey[i] >> 4;
		encryptionInfoOut[(i*2) + (keyBytes*2) + 1] = ephemeralPubKey[i] & 0x0f;
	}

	for(i = 0; i < signatureBytes; i += 1) {
		encryptionInfoOut[(i*2) + (keyBytes*4)] = signature[i] >> 4;
		encryptionInfoOut[(i*2) + (keyBytes*4) + 1] = signature[i] & 0x0f;
	}

	for(i = 0; i < IDBytes; i += 1) {
		encryptionInfoOut[(i*2) + (keyBytes*4) + (signatureBytes*2)] = ID[i] >> 4;
		encryptionInfoOut[(i*2) + (keyBytes*4) + (signatureBytes*2) + 1] = ID[i] & 0x0f;
	}

	//Necessary because ArduinoJSON is too wimpy to handle a mid-stream null-terminator
	for(i = 0; i < (SIZE_OF_ENCRYPTION_INFO_PAYLOAD - 1); i += 1) {
		if(encryptionInfoOut[i] < 0x0a) {
			encryptionInfoOut[i] += 48;
		} else {
			encryptionInfoOut[i] += 87;
		}
	}

	encryptionInfoOut[SIZE_OF_ENCRYPTION_INFO_PAYLOAD - 1] = '\n';
}


//Necessary because ArduinoJSON is too wimpy to handle a mid-stream null-terminator
void Networking::stringToHex(char* out, char* s, const unsigned short start, const unsigned short length) {
	for(unsigned short i = 0; i < length; i += 1) {
		if(48 <= s[(i*2) + start] && s[(i*2) + start] <= 57) {
			s[(i*2) + start] -= 48;
		} else {
			s[(i*2) + start] -= 87;
		}
		out[i] = s[(i*2) + start];
		out[i] <<= 4;

		if(48 <= s[(i*2) + start + 1] && s[(i*2) + start + 1] <= 57) {
			s[(i*2) + start + 1] -= 48;
		} else {
			s[(i*2) + start + 1] -= 87;
		}
		out[i] |= s[(i*2) + start + 1];
	}
}


//Necessary because ArduinoJSON is too wimpy to handle a mid-stream null-terminator
void Networking::convertEncryptionInfoPayload(char* DSAPubKeyOut, char* ephemeralPubKeyOut, char* signatureOut, char* IDOut, char* encryptionInfo) {
	stringToHex(DSAPubKeyOut, encryptionInfo, 0, keyBytes);
	stringToHex(ephemeralPubKeyOut, encryptionInfo, (keyBytes*2), keyBytes);
	stringToHex(signatureOut, encryptionInfo, (keyBytes*4), signatureBytes);
	stringToHex(IDOut, encryptionInfo, ((keyBytes*4) + (signatureBytes*2)), IDBytes);
}


bool Networking::connectToPeer(IPAddress& connectToIP) {
	Serial.println(F("connectToPeer"));

	const bool GENERATE_NEW_KEY = true;
	//									32				32					64				4			= 264
	pki.initialize(userDSAPrivateKey, userDSAPubKey, userEphemeralPubKey, userSignature, userID, GENERATE_NEW_KEY);
	createEncryptionInfoPayload(encryptionInfo, userDSAPubKey, userEphemeralPubKey, userSignature, userID);

	udp.begin(CONNECTION_PORT);

	const unsigned short outgoingPeerUniqueHandshakeValue = uuid + messagesSentCount;
//	messagesOut.enqueue(new Message(MESSAGE_TYPE::HANDSHAKE, new IdempotencyToken(outgoingPeerUniqueHandshakeValue, nowMS()), nullptr /*send encryption data*/, nullptr, &connectionEstablished));
	messagesOut.enqueue(new Message(MESSAGE_TYPE::HANDSHAKE, new IdempotencyToken(outgoingPeerUniqueHandshakeValue, nowMS()), copyString(encryptionInfo, MAX_MESSAGE_LENGTH) /*will this work? Who knows!*/, nullptr, &connectionEstablished));
	glEEpalInfo = new glEEpal(connectToIP, outgoingPeerUniqueHandshakeValue);
}


// Potential optimization: process more than one token per frame & make into time sensitive process
void Networking::removeExpiredIncomingIdempotencyToken() {
	QueueNode<IdempotencyToken>* nextTokenNode = messagesInIdempotencyTokens.peek();
	if(nextTokenNode == nullptr) {
		return;
	}

	if(nowMS() > nextTokenNode->getData()->getTimestamp() + INCOMING_IDEMPOTENCY_TOKEN_EXPIRED_THRESHOLD_MS) {
		messagesInIdempotencyTokens.dequeue();
		delete nextTokenNode;
	}
}


bool Networking::exceededMaxOutgoingTokenRetryCount() {
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

	udp.beginPacket(glEEpalInfo->getIPAddress(), CONNECTION_PORT);
	udp.write(outputBuffer);
	udp.endPacket();

	msg.getIdempotencyToken()->incrementRetryCount();
	messagesSentCount += 1;
}


bool Networking::processOutgoingMessageQueueNode(Queue<Message>& messagesOut, QueueNode<Message>* nextMessage) {
	if(nowMS() > nextMessage->getData()->getIdempotencyToken()->getTimestamp() + (nextMessage->getData()->getIdempotencyToken()->getRetryCount() * RESEND_OUTGOING_MESSAGE_THRESHOLD_MS)) {
		sendOutgoingMessage(*(nextMessage->getData()));

		nextMessage->getData()->doOutgoingPostProcess(messagesOut, *(nextMessage->getData()));
		return true;
	} else {
		return false;
	}
}


void Networking::sendHeartbeat() {
	sendOutgoingMessage(*heartbeat);
}


void Networking::checkHeartbeat() {
	approxCurrentTime = nowMS();

	if(approxCurrentTime - lastHeartbeatReceivedMS > FLATLINE_THRESHOLD_MS) {
		DebugLog::getLog().logError(NETWORK_HEARTBEAT_FLATLINE);
		dropConnection();
		return;
	}

	if(approxCurrentTime - lastHeartbeatSentMS > HEARTBEAT_RESEND_THRESHOLD_MS) {
		sendHeartbeat();
		lastHeartbeatSentMS = approxCurrentTime;
	}
}


void Networking::listenToHeartbeat(const unsigned long time) {
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
		listenToHeartbeat(nowMS());
	break;

	case MESSAGE_TYPE::CONFIRMATION:
		Serial.println(F("got confirmation of something"));
		
		messageOutWithMatchingIdempotencyToken = messagesOut.find(*msg.getData());
		if(messageOutWithMatchingIdempotencyToken) {
			messageOutWithMatchingIdempotencyToken->getData()->doConfirmedPostProcess(*this, messagesOut, msg); //In the case of a handshake, this is connectionEstablished(). In the case of a chat message, this is removeFromQueue()

		} else {
			DebugLog::getLog().logWarning(NETWORK_CONFIRMATION_NO_MATCH_FOUND);
		}
	break;

	//NOTE: ProcessIncomingMessageQueueNode will call Display function if message type is CHAT, adding ~1ms processing time
	case MESSAGE_TYPE::CHAT:
		messagesOut.enqueue(new Message(MESSAGE_TYPE::CONFIRMATION, new IdempotencyToken(msg.getData()->getIdempotencyToken()->getValue(), nowMS()), nullptr, /*nullptr,*/ &removeFromQueue, nullptr));

		if(!messagesInIdempotencyTokens.find(*(msg.getData()->getIdempotencyToken()))) {
			messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));
			(*chatMessageReceivedCallback)(msg.getData()->getChat());
		}
	break;

	case MESSAGE_TYPE::HANDSHAKE:
		Serial.println(F("got handshake"));

		messagesOut.enqueue(new Message(MESSAGE_TYPE::CONFIRMATION, new IdempotencyToken(msg.getData()->getIdempotencyToken()->getValue(), nowMS()), copyString(encryptionInfo, MAX_MESSAGE_LENGTH), /*nullptr,*/ &removeFromQueue, nullptr));

		Serial.println(F("conf to handsh enqued"));

		if(!messagesInIdempotencyTokens.find(*(msg.getData()->getIdempotencyToken()))) {
			messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));

			QueueNode<Message>* messageOutNode = messagesOut.peek();
			while(messageOutNode != nullptr) {
				if(messageOutNode->getData()->getIdempotencyToken()->getValue() == glEEpalInfo->getHandshakeIdempotencyTokenValue()) {
					break;
				}

				messageOutNode = messageOutNode->getNode();
			}

			if(messageOutNode) {
				connectionEstablished(*this, messagesOut, msg, *(messageOutNode->getData()));
			} else {
				DebugLog::getLog().logWarning(ERROR_CODE::NETWORK_UNEXPECTED_HANDSHAKE_FROM_CONNECTED_IP);
			}
		} else {
			DebugLog::getLog().logWarning(NETWORK_DUPLICATE_HANDSHAKE);
		}
	break;

	default:
		DebugLog::getLog().logWarning(NETWORK_UNKNOWN_INCOMING_MESSAGE_TYPE);
	break;
	}
}


bool Networking::processIncomingMessageQueueNode(Queue<Message>& messagesIn, QueueNode<Message>* nextMessage) {
	messagesIn.remove(*nextMessage);
	processIncomingMessage(*nextMessage);
	delete nextMessage;
	return true;
}


bool Networking::processQueue(bool (Networking::*processMessage)(Queue<Message>&, QueueNode<Message>*), Queue<Message>& fromQueue) {
	while(queueStartNode) {
		if(queueStartNode->getData()->getMessageType() == searchMessageType) {
			holdingNode = queueStartNode->getNode();
			if((this->*processMessage)(fromQueue, queueStartNode)) {
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
	packetSize = udp.parsePacket(); //destroys body of HTTPS responses (╯°□°）╯︵ ┻━┻
	if(packetSize > 0) {
		udp.read(messageBuffer, packetSize);
		if(*glEEpalInfo == udp.remoteIP()) { //look through list of glEEpals to find match with msg.getSender()
			//decrypt message

			messageReceivedCount += 1;

			StaticJsonDocument<JSON_DOCUMENT_SIZE> parsedDocument; //Maybe this could be a private member (reused) instead of constructing and destructing every time
			DeserializationError parsingError = deserializeJson(parsedDocument, messageBuffer, JSON_DOCUMENT_SIZE);
			if(parsingError) {
				//write data to buffer to check in gleemail.ino(?) in case HTTP GET is stored in UDP buffer (for when MCCP version info is requested)
				DebugLog::getLog().logError(JSON_MESSAGE_DESERIALIZATION_ERROR);
				return true;
			}

			intoQueue.enqueue(new Message(parsedDocument, nowMS(), *glEEpalInfo));
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
			Serial.println(processElapsedTime);
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
	if(!doTimeSensesitiveProcess(MAX_GET_MESSAGES_PROCESS_DURATION_MS, MAX_GET_MESSAGES_PROCESS_DURATION_MS, &Networking::getMessages, nullptr, messagesIn)) {
		//Maybe log error about get messages (specifically) being slow

		if(messageReceivedCount > MAX_MESSAGE_RECEIVED_COUNT) {
			DebugLog::getLog().logError(NETWORK_TOO_MANY_MESSAGES_RECEIVED);
			dropConnection();
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

	(this->*processHeartbeat)();

	queueStartNode = messagesOut.peek();
	if(queueStartNode) {
		searchMessageType = START_MESSAGE_TYPE;
		if(!doTimeSensesitiveProcess(processElapsedTime, MAX_PROCESS_OUTGOING_MESSAGE_QUEUE_DURATION_MS, &Networking::processQueue, &Networking::processOutgoingMessageQueueNode, messagesOut)) {
			//Maybe log error about process outgoing messages (specifically) being slow
			if(connected && exceededMaxOutgoingTokenRetryCount()) { //this is not safe for group chat because connected will be true after the first glEEconnection
				DebugLog::getLog().logError(NETWORK_OUTGOING_TOKEN_TIMESTAMP_ELAPSED);
				dropConnection();
			}
		}
	}

	removeExpiredIncomingIdempotencyToken();
}

#endif
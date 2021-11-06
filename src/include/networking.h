#ifndef NETWORKING_H
#define NETWORKING_H

#include "Arduino.h"

#include <WiFiUdp.h>
#include <ArduinoJson.hpp>

#include "global.h"
#include "queue.h"

#include "message.h"

#include "LiteChaCha/keyinfrastructure.h"
#include "LiteChaCha/authenticatedencrypt.h"


class Networking {
private:
	//unsigned short messageReceivedCount = 0; //TESTING ONLY! Put me back by the function definition for getMessages()

	bool& shutdownFlag;

	WiFiUDP udp;
	glEEpal* glEEpalInfo;

	bool connected = false;
	unsigned long connectedMS;

	static const constexpr unsigned short MAX_OUTGOING_MESSAGE_RETRY_COUNT = 10;
	static const constexpr unsigned short RESEND_OUTGOING_MESSAGE_THRESHOLD_MS = 500;
	static const constexpr unsigned short INCOMING_IDEMPOTENCY_TOKEN_EXPIRED_THRESHOLD_MS = (MAX_OUTGOING_MESSAGE_RETRY_COUNT * RESEND_OUTGOING_MESSAGE_THRESHOLD_MS) + RESEND_OUTGOING_MESSAGE_THRESHOLD_MS;
	static const constexpr unsigned short SIZE_OF_ENCRYPTION_INFO_PAYLOAD = 265; //32: DSAPubKey + 32: EphemeralPubKey + 64: signature + 4: ID + 1: nullterminator //Don't forget to move me if the rest of encryption is moved

	unsigned short uuid;
	void createuuid(char*);
	unsigned short messagesSentCount = 0;

	unsigned long (*nowMS)();
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
	void stringToHex(char*, const char*, const unsigned short, const unsigned short); // REMOVE ME?
	void convertEncryptionInfoPayload(char*, char*, char*, char*, const char*); // REMOVE ME?

	void clearAllQueues();
	void dropConnection();

	Message* heartbeat;
	unsigned long lastHeartbeatSentMS = 0;
	unsigned long lastHeartbeatReceivedMS = 0;
	static const constexpr unsigned short HEARTBEAT_RESEND_THRESHOLD_MS = 500;
	static const constexpr unsigned short FLATLINE_THRESHOLD_MS = 4 * HEARTBEAT_RESEND_THRESHOLD_MS;
	static const constexpr unsigned short HEARTBEAT_STILLBORN_THRESHOLD_MS = 15 * FLATLINE_THRESHOLD_MS;
	void sendHeartbeat();
	void checkHeartbeat();
	void checkHeartbeatStillborn();
	void checkHeartbeatFlatline();
	void dontCheckHeartbeat() {}
	void (Networking::*checkHeartbeatThreshold)() = &Networking::checkHeartbeatStillborn;
	void (Networking::*processHeartbeat)() = &Networking::dontCheckHeartbeat; //Switch to checkHeartbeat() on first heartbeat received

#warning "This is probably too big for most messages. Maybe create second buffer for handshakes?"
	char messageFromUDPBuffer[POST_ENCRYPTED_MESSAGE_INFO_MAX_MESSAGE_BUFFER_SIZE/*JSON_DOCUMENT_SIZE + tagBytes + sizeof(messageCount) + 1*/] = {0};
//	char* messageFromUDPBuffer = new char[JSON_DOCUMENT_SIZE];
	unsigned short packetSize = 0;

	Queue<Message> messagesIn;
	Queue<IdempotencyToken> messagesInIdempotencyTokens;
	Queue<Message> messagesOut;
	MESSAGE_TYPE searchMessageType;

	void createMessagePayload(char*, const size_t);
	void decryptBuffer(char*, const size_t);
	void encryptBufferAndPrepareMessagePayload(char*, const size_t);

	unsigned long long processStartTime = 0;
	unsigned short processElapsedTime = 0;

	unsigned short doTimeSensitiveProcess(const unsigned short, const unsigned short, bool (Networking::*)(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&), bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);

	//short doTimeSensetiveProcess(const short, const unsigned short, bool (Networking::*)(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&), bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);

	QueueNode<Message>* queueStartNode;
	QueueNode<Message>* holdingNode;
	QueueNode<Message>* messageOutWithMatchingIdempotencyToken;
	bool processQueue(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);
	bool processIncomingMessageQueueNode(Queue<Message>&, QueueNode<Message>*);
	static const constexpr unsigned short MAX_PROCESS_INCOMING_MESSAGE_QUEUE_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	void processIncomingError(QueueNode<Message>& msg);
	void processIncomingHeartbeat(QueueNode<Message>& msg);
	void processIncomingConfirmation(QueueNode<Message>& msg);
	void processIncomingChat(QueueNode<Message>& msg);
	void processIncomingHandshake(QueueNode<Message>& msg);
	
	void (*chatMessageReceivedCallback)(const char*);

	unsigned long messageResendTime(QueueNode<Message>& msg);
	bool processOutgoingMessageQueueNode(Queue<Message>&, QueueNode<Message>*);
	static const constexpr unsigned short MAX_PROCESS_OUTGOING_MESSAGE_QUEUE_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	bool getMessages(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);
	static const constexpr unsigned short MAX_GET_MESSAGES_PROCESS_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;
	unsigned short messageReceivedCount = 0;
	static const constexpr unsigned short MAX_MESSAGE_RECEIVED_COUNT = 10;

	Message& sendOutgoingMessage(Message&);

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
		if(n.connected == false) {
			Serial.println(F("beginning auth"));

			n.convertEncryptionInfoPayload(n.peerDSAPubKey, n.peerEphemeralPubKey, n.peerSignature, n.peerID, messageIn.getData()->getChat());

			if((n.pki.IDUnique(n.userID, n.peerID)) && (n.pki.signatureValid(n.peerDSAPubKey, n.peerEphemeralPubKey, n.peerSignature))) {
				n.pki.createSessionKey(n.peerEphemeralPubKey); // Creates a shared private session key, overwriting peerEphemeralPubKey, if both users have different IDs and the peer's signature is valid.
			} else {
				DebugLog::getLog().logError(ERROR_CODE::NETWORK_AUTHENTICATION_FAILED);
			}

			n.ae.initialize(n.peerEphemeralPubKey, n.userID, n.peerID);
			//Free to delete pki, keyBytes, signatureBytes, IDBytes, userDSAPrivateKey, userDSAPublicKey, peerDSAPublicKey, userEphemeralPubKey, peerEphemeralPubKey, userSignature, peerSignature, userID, peerID, encryptionInfo

			delete messagesOutQueue.remove(messageOut); //removes outgoing handshake from queue
			Serial.println(F("Connected to peer!"));

			n.connectedMS = n.nowMS();
			n.processHeartbeat = &Networking::checkHeartbeat;

			n.connected = true;
		} else {
			DebugLog::getLog().logWarning(ERROR_CODE::NETWORK_CONNECTION_ATTEMPT_WHILE_CONNECTED);
		}
	}
public:
	Networking(unsigned long (*)(), void (*)(const char*), const long u, bool& quit);
	~Networking();

	void processNetwork();
	void sendChatMessage(const char*);

	bool connectToPeer(IPAddress&);
};


Networking::Networking(unsigned long (*millis)(), void (*chatMsgCallback)(const char*), const long u, bool& quit) : shutdownFlag{quit} {
	nowMS = millis;
	uuid = u + nowMS(); //CHANGE ME!
	chatMessageReceivedCallback = chatMsgCallback;

	heartbeat = new Message(MESSAGE_TYPE::HEARTBEAT, new IdempotencyToken(0, 0), nullptr, /*nullptr,*/ nullptr, nullptr);

//#warning "Use correct buffer size here. POST_ENCRYPTED_MESSAGE_INFO_MAX_MESSAGE_BUFFER_SIZE is probably too big. Maybe create second buffer for handshakes?"
	/*for(int i = 0; i < POST_ENCRYPTED_MESSAGE_INFO_MAX_MESSAGE_BUFFER_SIZE; i += 1) {
		messageFromUDPBuffer[i] = '\0';
	}*/

	//tag = new char[tagBytes];
}


Networking::~Networking() {
	delete glEEpalInfo;
	//delete[] messageFromUDPBuffer;
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
// A lot of this is not necessary if we're going to shut down anyway.
/*	connected = false;
	processHeartbeat = &Networking::dontCheckHeartbeat;

	IPAddress palIP = glEEpalInfo->getIPAddress();
	delete glEEpalInfo;
	glEEpalInfo = nullptr;

	clearAllQueues();

	connectToPeer(palIP);*/

	Serial.println(F("Connection dropped!"));

	shutdownFlag = true;
}


void Networking::sendChatMessage(const char* chat) {
	messagesOut.enqueue(new Message(MESSAGE_TYPE::CHAT, new IdempotencyToken(uuid + messagesSentCount, nowMS()), copyString(chat, MAX_MESSAGE_LENGTH)/*, nullptr*/, nullptr, &removeFromQueue));
}


void Networking::createuuid(char* userID) {
	Serial.print(F("userID:"));
	for(unsigned short i = 0; i < IDBytes; i += 1) {
		Serial.print(' ');
		Serial.print(userID[i], HEX);
	}
	Serial.println();

	uuid |= userID[0] << 8;
	uuid |= userID[1];
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

	encryptionInfoOut[SIZE_OF_ENCRYPTION_INFO_PAYLOAD - 1] = '\0';
}


//Necessary because ArduinoJSON is too wimpy to handle a mid-stream null-terminator
void Networking::stringToHex(char* out, const char* s, const unsigned short start, const unsigned short length) {
/*	for(unsigned short i = 0; i < length; i += 1) {
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
	}*/

	for(unsigned short i = 0; i < length; i += 1) {
		if(48 <= s[(i*2) + start] && s[(i*2) + start] <= 57) {
			out[i] = (s[(i*2) + start] - 48) << 4;
		} else {
			out[i] = (s[(i*2) + start] - 87) << 4;
		}

		if(48 <= s[(i*2) + start + 1] && s[(i*2) + start + 1] <= 57) {
			out[i] |= (s[(i*2) + start + 1] - 48);
		} else {
			out[i] |= (s[(i*2) + start + 1] - 87);
		}
	}
}


//Necessary because ArduinoJSON is too wimpy to handle a mid-stream null-terminator
void Networking::convertEncryptionInfoPayload(char* DSAPubKeyOut, char* ephemeralPubKeyOut, char* signatureOut, char* IDOut, const char* encryptionInfo) {
	stringToHex(DSAPubKeyOut, encryptionInfo, 0, keyBytes);
	stringToHex(ephemeralPubKeyOut, encryptionInfo, (keyBytes*2), keyBytes);
	stringToHex(signatureOut, encryptionInfo, (keyBytes*4), signatureBytes);
	stringToHex(IDOut, encryptionInfo, ((keyBytes*4) + (signatureBytes*2)), IDBytes);
}


bool Networking::connectToPeer(IPAddress& connectToIP) {
	const bool GENERATE_NEW_KEY = true;
	pki.initialize(userDSAPrivateKey, userDSAPubKey, userEphemeralPubKey, userSignature, userID, GENERATE_NEW_KEY);
	createuuid(userID);
	createEncryptionInfoPayload(encryptionInfo, userDSAPubKey, userEphemeralPubKey, userSignature, userID);

	udp.begin(CONNECTION_PORT);

	const unsigned short outgoingPeerUniqueHandshakeValue = uuid + messagesSentCount;
	Serial.print(F("connectToPeer: outgoing handshake idempotency token value: "));
	Serial.println(outgoingPeerUniqueHandshakeValue);

	messagesOut.enqueue(new Message(MESSAGE_TYPE::HANDSHAKE, new IdempotencyToken(outgoingPeerUniqueHandshakeValue, nowMS()), copyString(encryptionInfo, MAX_MESSAGE_LENGTH), nullptr, &connectionEstablished));
	glEEpalInfo = new glEEpal(connectToIP, outgoingPeerUniqueHandshakeValue);

	Serial.print(F("glEEpal IP: "));
	Serial.println(glEEpalInfo->getIPAddress());
	Serial.print(F("glEEpal Token: "));
	Serial.println(glEEpalInfo->getHandshakeIdempotencyTokenValue());

	return true;
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


void Networking::createMessagePayload(char* message, const size_t length) {// Optimize me!?
	unsigned short i;
	char tempMessageBuffer[length]; // Really don't want to have to do this!

	for(i = 0; i < length; i += 1) {
		tempMessageBuffer[i] = message[i];
	}

	for(i = 0; i < 8; i += 1) {
		message[i] = messageCount >> ((7 - i)*8); // I think this will work...
	}

	for(i = 0; i < tagBytes; i += 1) {
		message[i + 8] = tag[i];
	}

	for(i = 0; i < length; i += 1) {
		message[i + 8 + tagBytes] = tempMessageBuffer[i];
	}

	message[8 + tagBytes + length] = '\0'; // This terminator should terminate the udp buffer, I believe.

/*	unsigned short i = 0;
	for(i = 0; i < sizeof(messageCount); i += 1) { // Should we maybe have a standalone variable for sizeof(messageCount)?
		message[i*2] = (messageCount >> (60 - (i*8))) & 0x0f;
		message[(i*2) + 1] = (messageCount >> 56 - (i*8)) & 0x0f;
	}

	for(i = 0; i < tagBytes; i += 1) {
		message[(i*2) + (sizeof(messageCount)*2)] = tag[i] >> 4;
		message[(i*2) + (sizeof(messageCount)*2) + 1] = tag[i] & 0x0f;
	}

	for(i = 0; i < length; i += 1) {
		//Uh-oh! We are overwriting message!!!
	}
*/
/*
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
*/
}


void Networking::decryptBuffer(char* inputBuffer, const size_t length) {

}


void Networking::encryptBufferAndPrepareMessagePayload(char* outputBuffer, const size_t length) {
	ae.encryptAndTagMessage(messageCount, tag, outputBuffer, length);

	createMessagePayload(outputBuffer, length);
}


Message& Networking::sendOutgoingMessage(Message& msg) {
//	char outputBuffer[JSON_DOCUMENT_SIZE + tagBytes + sizeof(messageCount) + 1]; // Does this need to be 1 longer to match udp.write size?
//	char outputBuffer[((JSON_DOCUMENT_SIZE + 1 + tagBytes + sizeof(messageCount)) * 2) + 1]; // OOF. PLEASE KILL ME!
#warning "This is probably too big! Use correctly sized buffer (or make buffer a member variable of the Networking class?"
	char outputBuffer[POST_ENCRYPTED_MESSAGE_INFO_MAX_MESSAGE_BUFFER_SIZE]; // OOF. PLEASE KILL ME!
	StaticJsonDocument<OUTGOING_JSON_DOCUMENT_SIZE> doc;
// So it is + tagBytes (16 bytes) + sizeof(messageCount) I do want to confirm that it is indeed 8, but we don't need to do that right now.
	doc["T"] = static_cast<unsigned short>(msg.getMessageType());
	doc["I"] = msg.getIdempotencyToken()->getValue();
	doc["C"] = msg.getChat();

	//For sending error subobjects
	/*JsonObject E = doc.createNestedObject("E");
	E["D"] = static_cast<unsigned short>(msg.getError()->getID());
	E["A"] = msg.getError()->getAttribute();*/

	serializeJson(doc, outputBuffer);
	//if(msg.getMessageType() != MESSAGE_TYPE::HANDSHAKE) {
	if(connected) { //This is only slightly dissapointing because its less clear than checking for message type (only want to send handshakes and confirmations of handshakes unencrypted)
		encryptBufferAndPrepareMessagePayload(outputBuffer, measureJson(doc) + 1);	//DualJustice added one to PRE_ENCRYPTED_MESSAGE_INFO_MAX_MESSAGE_BUFFER_SIZE in global.h because we are adding one here.
	}

//	Serial.println(F("Sending:"));
//	Serial.println(outputBuffer);

	udp.beginPacket(glEEpalInfo->getIPAddress(), CONNECTION_PORT);
//	udp.write(outputBuffer, measureJson(doc) + 1 + tagBytes + sizeof(messageCount) + 1);
	/*const unsigned short wroteLength =*/ //udp.write(outputBuffer, ((measureJson(doc) + 1 + tagBytes + sizeof(messageCount)) * 2) + 1);
	udp.write(outputBuffer); // Just curious, does udp.write simply write the entire outputBuffer? If so, encrypted messages might pose a problem, as there is no simple way to determine when they terminate besides possibly sending the message length.
	udp.endPacket();

	/*Serial.print(F("Wrote: "));
	Serial.println(wroteLength);
	Serial.print(F("Max size of array: "));
	Serial.println(((measureJson(doc) + 1 + tagBytes + sizeof(messageCount)) * 2) + 1);*/

	msg.getIdempotencyToken()->incrementRetryCount();
	messagesSentCount += 1;

	return msg;
}


unsigned long Networking::messageResendTime(QueueNode<Message>& msg) {
	return msg.getData()->getIdempotencyToken()->getTimestamp() + (msg.getData()->getIdempotencyToken()->getRetryCount() * RESEND_OUTGOING_MESSAGE_THRESHOLD_MS);
}


bool Networking::processOutgoingMessageQueueNode(Queue<Message>& messagesOut, QueueNode<Message>* nextMessage) {
	if(nowMS() > messageResendTime(*nextMessage)) { //Probably shouldn't dereference here, I think QueueNode<Message> should be passed in as a reference instead. However, some refactoring will be necessary because probably all of the parameters should be references- catch a nullptr early and treat everything else as a ref.
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


void Networking::checkHeartbeatStillborn() {
	if(approxCurrentTime - connectedMS > HEARTBEAT_STILLBORN_THRESHOLD_MS) {
		DebugLog::getLog().logError(NETWORK_HEARTBEAT_STILLBORN);
		Serial.println(F("heartbeat stillborn"));
		dropConnection();
	}
}


void Networking::checkHeartbeatFlatline() {
	if(approxCurrentTime - lastHeartbeatReceivedMS > FLATLINE_THRESHOLD_MS) {
		DebugLog::getLog().logError(NETWORK_HEARTBEAT_FLATLINE);
		Serial.println(F("oopsies beeeeeeeeeeeeep"));
		dropConnection();
	}
}


void Networking::checkHeartbeat() {
	approxCurrentTime = nowMS();
	(this->*checkHeartbeatThreshold)();

	if(approxCurrentTime - lastHeartbeatSentMS > HEARTBEAT_RESEND_THRESHOLD_MS) {
		sendHeartbeat();
		lastHeartbeatSentMS = approxCurrentTime;
	}
}


void Networking::processIncomingError(QueueNode<Message>& msg) {
	//check for unique idempotency token
	messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));
	//DebugLog::getLog().logWarning(msg.getData()->getError()->getID());
}


void Networking::processIncomingHeartbeat(QueueNode<Message>& msg) {
	if(checkHeartbeatThreshold == &Networking::checkHeartbeatStillborn) {
		checkHeartbeatThreshold = &Networking::checkHeartbeatFlatline;
	}

	lastHeartbeatReceivedMS = nowMS();
}


void Networking::processIncomingConfirmation(QueueNode<Message>& msg) {
//	Serial.println(F("got confirmation of something"));

	messageOutWithMatchingIdempotencyToken = messagesOut.find(*msg.getData());
	if(messageOutWithMatchingIdempotencyToken) {
		messageOutWithMatchingIdempotencyToken->getData()->doConfirmedPostProcess(*this, messagesOut, msg); //In the case of a handshake, this is connectionEstablished(). In the case of a chat or confirmation, this is removeFromQueue()

	} else {
//		Serial.print(F("confirmation no match found idempotency token: "));
//		Serial.println(msg.getData()->getIdempotencyToken()->getValue());

		DebugLog::getLog().logWarning(NETWORK_CONFIRMATION_NO_MATCH_FOUND);
	}
}


//NOTE: ProcessIncomingMessageQueueNode will call Display function if message type is CHAT, adding ~1ms processing time
void Networking::processIncomingChat(QueueNode<Message>& msg) {
	messagesOut.enqueue(new Message(MESSAGE_TYPE::CONFIRMATION, new IdempotencyToken(msg.getData()->getIdempotencyToken()->getValue(), nowMS()), nullptr, /*nullptr,*/ &removeFromQueue, nullptr));

	if(!messagesInIdempotencyTokens.find(*(msg.getData()->getIdempotencyToken()))) {
		messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));
		(*chatMessageReceivedCallback)(msg.getData()->getChat());
	}
}


void Networking::processIncomingHandshake(QueueNode<Message>& msg) {
	//messagesOut.enqueue(new Message(MESSAGE_TYPE::CONFIRMATION, new IdempotencyToken(msg.getData()->getIdempotencyToken()->getValue(), nowMS()), copyString(encryptionInfo, MAX_MESSAGE_LENGTH), /*nullptr,*/ &removeFromQueue, nullptr)); //Send immediately instead of enqueuing message for timing

//	Serial.println(F("Received handshake"));

#warning "Any subsequent confirmation of a handshake after the first will be encrypted because connected is set to true after this"
	delete &sendOutgoingMessage(*new Message(MESSAGE_TYPE::CONFIRMATION, new IdempotencyToken(msg.getData()->getIdempotencyToken()->getValue(), nowMS()), copyString(encryptionInfo, MAX_MESSAGE_LENGTH), /*nullptr,*/ /*&removeFromQueue*/ nullptr /*Note: message is deleted here- outgoingPostProcess will never be called*/, nullptr)); //Not immediately logical, but this is correct.

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
}


void Networking::processIncomingMessage(QueueNode<Message>& msg) {
	switch(msg.getData()->getMessageType()) {
	case MESSAGE_TYPE::ERROR:
		processIncomingError(msg);
	break;

	case MESSAGE_TYPE::HEARTBEAT:
		processIncomingHeartbeat(msg);
	break;

	case MESSAGE_TYPE::CONFIRMATION:
		processIncomingConfirmation(msg);
	break;
	
	case MESSAGE_TYPE::CHAT:
		processIncomingChat(msg);
	break;

	case MESSAGE_TYPE::HANDSHAKE:
//		Serial.println(F("before process incoming handshake"));
		processIncomingHandshake(msg);
//		Serial.println(F("after process incoming handshake"));
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
//	Serial.println(F("Deleted processed message node"));
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
		udp.read(messageFromUDPBuffer, packetSize); // Depending on what packetSize is, the length of the ciphertext may need to be sent with the messagePayload!
		messageFromUDPBuffer[packetSize++] = '\0'; // Is this writing a null terminator somewhere it shouldn't with handshakes? Also, shouldn't a null terminator be put at the end of every packet by us anyways?
		if(*glEEpalInfo == udp.remoteIP()) { //group chat: search through list of glEEpals to find match

			//decrypt message !!
			if(connected) { //This is only slightly dissapointing because its less clear than checking for message type (only want to send handshakes and confirmations of handshakes unencrypted)
				decryptBuffer(messageFromUDPBuffer, packetSize);
				//encryptBufferAndPrepareMessagePayload(outputBuffer, measureJson(doc) + 1);	//DualJustice added one to PRE_ENCRYPTED_MESSAGE_INFO_MAX_MESSAGE_BUFFER_SIZE in global.h because we are adding one here.
			}

			//Serial.println(F("Receiving:"));
			//Serial.println(messageFromUDPBuffer);

			messageReceivedCount += 1;

			StaticJsonDocument<INCOMING_JSON_DOCUMENT_SIZE> parsedDocument; //Maybe this could be a private member (reused) instead of constructing and destructing every time
			DeserializationError parsingError = deserializeJson(parsedDocument, messageFromUDPBuffer);
			if(parsingError) {
				Serial.print(F("getMessages: parse error: "));
				Serial.println(parsingError.c_str());
				//write data to buffer to check in gleemail.ino(?) in case HTTP GET is stored in UDP buffer (for when MCCP version info is requested)
				DebugLog::getLog().logError(JSON_MESSAGE_DESERIALIZATION_ERROR);
				return true;
			}

			QueueNode<Message>* enqueuedQueueNode = intoQueue.enqueue(new Message(parsedDocument, nowMS(), *glEEpalInfo));
		} else {
			DebugLog::getLog().logWarning(NETWORK_UNKNOWN_MESSAGE_SENDER);
		}

		return true;
	} else {
		return false;
	}
}


unsigned short Networking::doTimeSensitiveProcess(const unsigned short previousProcessElapsedTime, const unsigned short MAX_PROCESSING_TIME, bool (Networking::*doProcess)(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&), bool (Networking::*passProcess)(Queue<Message>&, QueueNode<Message>*), Queue<Message>& onQueue) {
	processStartTime = nowMS();
	while(nowMS() - processStartTime < MAX_PROCESSING_TIME + (MAX_PROCESSING_TIME - previousProcessElapsedTime)) {
		if(!(this->*doProcess)(passProcess, onQueue)) {
			break;
		}
	}

	processElapsedTime = nowMS() - processStartTime;
	return processElapsedTime;
}


void Networking::processNetwork() {
	if(doTimeSensitiveProcess(MAX_GET_MESSAGES_PROCESS_DURATION_MS, MAX_GET_MESSAGES_PROCESS_DURATION_MS, &Networking::getMessages, nullptr, messagesIn) > MAX_GET_MESSAGES_PROCESS_DURATION_MS) {
		if(processElapsedTime > 2 * MAX_GET_MESSAGES_PROCESS_DURATION_MS) {
			DebugLog::getLog().logError(NETWORK_GET_MESSAGES_EXCEEDED_ALLOCATED_TIME_SIGNIFICANT);
		} else {
			DebugLog::getLog().logWarning(NETWORK_GET_MESSAGES_EXCEEDED_ALLOCATED_TIME_INSIGNIFICANT);
		}

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
		if(doTimeSensitiveProcess(processElapsedTime, MAX_PROCESS_INCOMING_MESSAGE_QUEUE_DURATION_MS, &Networking::processQueue, &Networking::processIncomingMessageQueueNode, messagesIn) > MAX_PROCESS_INCOMING_MESSAGE_QUEUE_DURATION_MS) {
			if(processElapsedTime > 2 * MAX_PROCESS_INCOMING_MESSAGE_QUEUE_DURATION_MS) {
				DebugLog::getLog().logError(NETWORK_MESSAGES_IN_EXCEEDED_ALLOCATED_TIME_SIGNIFICANT);
			} else {
				DebugLog::getLog().logWarning(NETWORK_MESSAGES_IN_EXCEEDED_ALLOCATED_TIME_INSIGNIFICANT);
			}
		}
	}

	(this->*processHeartbeat)();

	queueStartNode = messagesOut.peek();
	if(queueStartNode) {
		searchMessageType = START_MESSAGE_TYPE;
		if(doTimeSensitiveProcess(processElapsedTime, MAX_PROCESS_OUTGOING_MESSAGE_QUEUE_DURATION_MS, &Networking::processQueue, &Networking::processOutgoingMessageQueueNode, messagesOut) > MAX_PROCESS_OUTGOING_MESSAGE_QUEUE_DURATION_MS) {
			if(processElapsedTime > 2 * MAX_PROCESS_OUTGOING_MESSAGE_QUEUE_DURATION_MS) {
				DebugLog::getLog().logError(NETWORK_MESSAGES_OUT_EXCEEDED_ALLOCATED_TIME_SIGNIFICANT);
			} else {
				DebugLog::getLog().logWarning(NETWORK_MESSAGES_OUT_EXCEEDED_ALLOCATED_TIME_INSIGNIFICANT);
			}

			if(connected && exceededMaxOutgoingTokenRetryCount()) { //this is not safe for group chat because connected will be true after the first glEEconnection
				DebugLog::getLog().logError(NETWORK_OUTGOING_TOKEN_TIMESTAMP_ELAPSED);
				dropConnection();
			}
		}
	}

	removeExpiredIncomingIdempotencyToken();
}

#endif
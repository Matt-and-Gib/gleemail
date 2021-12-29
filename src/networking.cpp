#include "include/networking.h"

#include "Arduino.h"


#include <ArduinoJson.hpp>

#include "include/message.h"


using namespace GLEEMAIL_DEBUG;
using namespace ArduinoJson;


Networking::Networking(unsigned long (*millis)(), void (*chatMsgCallback)(char*), void (*connectedUpdateDisplay)(), const long u, bool& quit) : shutdownFlag{quit} {
	nowMS = millis;
	uuid = u + nowMS(); //CHANGE ME!
	chatMessageReceivedCallback = chatMsgCallback;
	connectedToPeerClearDisplay = connectedUpdateDisplay;

	heartbeat = new Message(MESSAGE_TYPE::HEARTBEAT, new IdempotencyToken(0, 0), nullptr, /*nullptr,*/ nullptr, nullptr);
}


Networking::~Networking() {
	delete glEEpalInfo;
}


void Networking::clearAllQueues() {
	delete messagesIn.peek();
	delete messagesInIdempotencyTokens.peek();
	delete messagesOut.peek();

	queueStartNode = nullptr;
	holdingNode = nullptr;
	messageOutWithMatchingIdempotencyToken = nullptr;
}


void Networking::dropConnection() {
	shutdownFlag = true;
}


void Networking::removeFromQueue(Queue<Message>& fromQueue, Message& node) {
	delete fromQueue.remove(node);
}


void Networking::removeFromQueue(Networking& n, Queue<Message>& messagesOutQueue, QueueNode<Message>& messageIn, Message& messageOut) {
	removeFromQueue(messagesOutQueue, messageOut);
}


void Networking::connectionEstablished(Networking& n, Queue<Message>& messagesOutQueue, QueueNode<Message>& messageIn, Message& messageOut) {
	if(n.connected == false) {
		Serial.println(F("Authenticating Peer..."));

		n.convertEncryptionInfoPayload(n.peerDSAPubKey, n.peerEphemeralPubKey, n.peerSignature, n.peerID, messageIn.getData()->getChat());

		if((n.pki.IDUnique(n.userID, n.peerID)) && (n.pki.signatureValid(n.peerDSAPubKey, n.peerEphemeralPubKey, n.peerSignature))) {
			n.pki.createSessionKey(n.peerEphemeralPubKey); // Creates a shared private session key, overwriting peerEphemeralPubKey, if both users have different IDs and the peer's signature is valid.
		} else {
			DebugLog::getLog().logError(ERROR_CODE::NETWORK_AUTHENTICATION_FAILED);
		}

		n.ae.initialize(n.peerEphemeralPubKey, n.userID, n.peerID);
		//Free to delete pki, keyBytes, signatureBytes, IDBytes, userDSAPrivateKey, userDSAPublicKey, peerDSAPublicKey, userEphemeralPubKey, peerEphemeralPubKey, userSignature, peerSignature, userID, peerID, encryptionInfo

		if(DebugLog::getLog().verboseMode()) {
			Serial.print(F("Peer DSA Public Key:"));
			for(unsigned short i = 0; i < keyBytes; i += 1) {
				Serial.print(' ');
				if(n.peerDSAPubKey[i] > 0x0f) {
					Serial.print(n.peerDSAPubKey[i], HEX);
				} else {
					Serial.print('0');
					Serial.print(n.peerDSAPubKey[i], HEX);
				}
			}
			Serial.println();
		}

		delete messagesOutQueue.remove(messageOut); //removes outgoing handshake from queue

		n.connectedMS = n.nowMS();
		n.processHeartbeat = &Networking::checkHeartbeat;

		n.connected = true;
	} else {
		DebugLog::getLog().logWarning(ERROR_CODE::NETWORK_CONNECTION_ATTEMPT_WHILE_CONNECTED);
	}
}


void Networking::sendChatMessage(const char* chat) {
	messagesOut.enqueue(new Message(MESSAGE_TYPE::CHAT, new IdempotencyToken(uuid + messagesSentCount, nowMS()), copyString(chat, MAX_MESSAGE_LENGTH)/*, nullptr*/, nullptr, &removeFromQueue));
}


void Networking::createuuid(char* userID) {
	uuid |= userID[0] << 8;
	uuid |= userID[1];
}


void Networking::buildEncryptionInfoPayload(char* encryptionInfoOut, const char* DSAPubKey, const char* ephemeralPubKey, const char* signature, const char* ID) {
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
	buildEncryptionInfoPayload(encryptionInfo, userDSAPubKey, userEphemeralPubKey, userSignature, userID);

	if(DebugLog::getLog().verboseMode()) {
		Serial.print(F("User DSA Public Key:"));
		for(unsigned short i = 0; i < keyBytes; i += 1) {
			Serial.print(' ');
			if(userDSAPubKey[i] > 0x0f) {
				Serial.print(userDSAPubKey[i], HEX);
			} else {
				Serial.print('0');
				Serial.print(userDSAPubKey[i], HEX);
			}
		}
	}
	Serial.println();

	udp.begin(CONNECTION_PORT);

	const unsigned short outgoingPeerUniqueHandshakeValue = uuid + messagesSentCount;
	messagesOut.enqueue(new Message(MESSAGE_TYPE::HANDSHAKE, new IdempotencyToken(outgoingPeerUniqueHandshakeValue, nowMS()), copyString(encryptionInfo, MAX_MESSAGE_LENGTH), nullptr, &connectionEstablished));
	glEEpalInfo = new glEEpal(connectToIP, outgoingPeerUniqueHandshakeValue);

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
		message[i] = messageCount >> ((7 - i)*8);
	}

	for(i = 0; i < tagBytes; i += 1) {
		message[i + 8] = tag[i];
	}

	for(i = 0; i < length; i += 1) {
		message[i + 8 + tagBytes] = tempMessageBuffer[i];
	}

	message[8 + tagBytes + length] = '\0';
}


void Networking::encryptBufferAndPrepareMessagePayload(char* outputBuffer, const size_t length) {
	ae.encryptAndTagMessage(messageCount, tag, outputBuffer, length);
}


void Networking::buildAuthenticationPayload(char* authPayload) {
	unsigned short i;

	for(i = 0; i < 16; i += 1) {
		authPayload[i] = (messageCount >> ((15 - i)*4)) & 0x0f;
		authPayload[(i*2) + 16] = tag[i] >> 4;
		authPayload[(i*2) + 16 + 1] = tag[i] & 0x0f;
	}

	//Necessary because ArduinoJSON is too wimpy to handle a mid-stream null-terminator
	for(i = 0; i < 48; i += 1) {
		if(authPayload[i] < 0x0a) {
			authPayload[i] += 48;
		} else {
			authPayload[i] += 87;
		}
	}

	authPayload[48] = '\0';
}


void Networking::prepareOutgoingEncryptedChat(char* cipherText, unsigned short chatBytes) {
	unsigned short i;

	for(i = 0; i < chatBytes; i += 1) {
		cipherText[((chatBytes*2) - 1) - (i*2)] = cipherText[((chatBytes - i) - 1)] & 0x0f;
		cipherText[((chatBytes*2) - 2) - (i*2)] = cipherText[((chatBytes - i) - 1)] >> 4;
	}

	//Necessary because ArduinoJSON is too wimpy to handle a mid-stream null-terminator
	for(i = 0; i < chatBytes*2; i += 1) {
		if(cipherText[i] < 0x0a) {
			cipherText[i] += 48;
		} else {
			cipherText[i] += 87;
		}
	}

	cipherText[chatBytes*2] = '\0';
}


Message& Networking::sendOutgoingMessage(Message& msg) {
	StaticJsonDocument<OUTGOING_JSON_DOCUMENT_SIZE> doc; //maybe make me a member of the class instead!
	doc["T"] = static_cast<unsigned short>(msg.getMessageType());
	doc["I"] = msg.getIdempotencyToken()->getValue();

	char* encryptedChat = nullptr;
	char* authenticationPayload = nullptr;
	if((msg.getMessageType() == MESSAGE_TYPE::CHAT) && connected) { //the && connected might be redundant.
		encryptedChat = new char[(2 * msg.getChatLength()) + TERMINATOR];
		overwriteString(msg.getChat(), msg.getChatLength(), encryptedChat);
		ae.encryptAndTagMessage(messageCount, tag, encryptedChat, msg.getChatLength());
		prepareOutgoingEncryptedChat(encryptedChat, msg.getChatLength());

		doc["C"] = encryptedChat;

		authenticationPayload = new char[AUTHENTICATION_PAYLOAD_SIZE];
		buildAuthenticationPayload(authenticationPayload);
		doc["G"] = authenticationPayload; // This is messageCount (16 bytes) and tag (32 bytes), as equivalent string values, and a null terminator.
	} else {
		doc["C"] = msg.getChat();
	}

	//For sending error subobjects
	/*JsonObject E = doc.createNestedObject("E");
	E["D"] = static_cast<unsigned short>(msg.getError()->getID());
	E["A"] = msg.getError()->getAttribute();*/

	serializeJson(doc, outgoingMessageBuffer);

	udp.beginPacket(glEEpalInfo->getIPAddress(), CONNECTION_PORT);
	udp.write(outgoingMessageBuffer);
	udp.endPacket();

	msg.getIdempotencyToken()->incrementRetryCount();
	messagesSentCount += 1;

	delete[] authenticationPayload;
	delete[] encryptedChat;

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
		dropConnection();
	}
}


void Networking::checkHeartbeatFlatline() {
	if(approxCurrentTime - lastHeartbeatReceivedMS > FLATLINE_THRESHOLD_MS) {
		DebugLog::getLog().logError(NETWORK_HEARTBEAT_FLATLINE);
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
		Serial.println(F("Connected to Peer!"));
		(*connectedToPeerClearDisplay)();
	}

	lastHeartbeatReceivedMS = nowMS();
}


void Networking::processIncomingConfirmation(QueueNode<Message>& msg) {
	messageOutWithMatchingIdempotencyToken = messagesOut.find(*msg.getData());
	if(messageOutWithMatchingIdempotencyToken) {
		messageOutWithMatchingIdempotencyToken->getData()->doConfirmedPostProcess(*this, messagesOut, msg); //In the case of a handshake, this is connectionEstablished(). In the case of a chat or confirmation, this is removeFromQueue()
	} else {
		DebugLog::getLog().logWarning(NETWORK_CONFIRMATION_NO_MATCH_FOUND);
	}
}


void Networking::stringToULL() {
	messageCount = 0;

	for(unsigned short i = 0; i < SIZE_OF_UNSIGNED_LONG_LONG; i += 1) {
		messageCount |= tag[i] << ((SIZE_OF_UNSIGNED_LONG_LONG - i) - 1)*BITS_PER_BYTE;
	}
}


char* Networking::decryptChat(Message& msg) {
	stringToHex(tag, msg.getAuthentication(), 0, SIZE_OF_UNSIGNED_LONG_LONG);
	stringToULL();
	stringToHex(tag, msg.getAuthentication(), (SIZE_OF_UNSIGNED_LONG_LONG*2), tagBytes);

	unsigned short decryptedMessageLength = msg.getChatLength() / 2;

	char* chatDecryptionBuffer = new char[msg.getChatLength() + TERMINATOR]; //delete[]'ed by updateDispaly() in main
	stringToHex(chatDecryptionBuffer, msg.getChat(), 0, msg.getChatLength());
	chatDecryptionBuffer[decryptedMessageLength] = '\0';

	if(ae.messageAuthentic(chatDecryptionBuffer, decryptedMessageLength, messageCount, tag)) { // Authenticates the message with the MAC tag.
		ae.decryptAuthenticatedMessage(chatDecryptionBuffer, decryptedMessageLength, messageCount); // Decrypts message, overwriting it with the plaintext.
	} else {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_RECEIVED_UNAUTHENTIC_MESSAGE);
		chatDecryptionBuffer[0] = '\0';
	}

	return chatDecryptionBuffer;
}


void Networking::processIncomingChat(QueueNode<Message>& msg) {
	messagesOut.enqueue(new Message(MESSAGE_TYPE::CONFIRMATION, new IdempotencyToken(msg.getData()->getIdempotencyToken()->getValue(), nowMS()), nullptr, /*nullptr,*/ &removeFromQueue, nullptr));

	if(!messagesInIdempotencyTokens.find(*(msg.getData()->getIdempotencyToken()))) {
		messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));
		char* tempDecryptedChat = decryptChat(*msg.getData()); //once decryption is verified, use the function as the parameter for the chat callback and don't create this pointer.
		(*chatMessageReceivedCallback)(tempDecryptedChat);
	}
}


void Networking::processIncomingHandshake(QueueNode<Message>& msg) {
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
		processIncomingHandshake(msg);
	break;

	default:
		DebugLog::getLog().logError(NETWORK_UNKNOWN_INCOMING_MESSAGE_TYPE);
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
		udp.read(messageFromUDPBuffer, packetSize);
#warning "remove this terminator once we can send a message based on length instead of \0"
		messageFromUDPBuffer[packetSize++] = '\0'; // Is this writing a null terminator somewhere it shouldn't with handshakes? Also, shouldn't a null terminator be put at the end of every packet by us anyways?
		if(*glEEpalInfo == udp.remoteIP()) { //group chat: search through list of glEEpals to find match
			messageReceivedCount += 1;

			StaticJsonDocument<INCOMING_JSON_DOCUMENT_SIZE> parsedDocument; //Maybe this could be a private member (reused) instead of constructing and destructing every time
			DeserializationError parsingError = deserializeJson(parsedDocument, messageFromUDPBuffer);
			if(parsingError) {
				//Serial.print(F("getMessages: parse error: "));
				//Serial.println(parsingError.c_str());
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


unsigned short Networking::doTimeSensitiveProcess(const unsigned int previousProcessElapsedTime, const unsigned short MAX_PROCESSING_TIME, bool (Networking::*doProcess)(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&), bool (Networking::*passProcess)(Queue<Message>&, QueueNode<Message>*), Queue<Message>& onQueue) {
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
#ifndef NETWORKING_H
#define NETWORKING_H


#include <WiFiUdp.h>

#include "global.h"
#include "queue.h"

#include "LiteChaCha/keyinfrastructure.h" //Could be in source if objects were pointers
#include "LiteChaCha/authenticatedencrypt.h" //Could be in source if objects were pointers


class glEEpal;
class Message;
class IdempotencyToken;


class Networking final {
private:
	bool& shutdownFlag;

	WiFiUDP udp;
	glEEpal* glEEpalInfo;

	bool connected = false;
	unsigned long connectedMS;
	void (*connectedToPeerClearDisplay)();

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


	void buildEncryptionInfoPayload(char*, const char*, const char*, const char*, const char*); // REMOVE ME?
	void stringToHex(char*, const char*, const unsigned short, const unsigned short); // REMOVE ME?
	void convertEncryptionInfoPayload(char*, char*, char*, char*, const char*); // REMOVE ME?
	void stringToULL();

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
	char messageFromUDPBuffer[MESSAGE_BUFFER_SIZE] = {0};
	unsigned short packetSize = 0;

	Queue<Message> messagesIn;
	Queue<IdempotencyToken> messagesInIdempotencyTokens;
	Queue<Message> messagesOut;
	MESSAGE_TYPE searchMessageType;

	void createMessagePayload(char*, const size_t);
	void encryptBufferAndPrepareMessagePayload(char*, const size_t);

	unsigned long processStartTime = 0;
	unsigned short processElapsedTime = 0;

	unsigned short doTimeSensitiveProcess(const unsigned int, const unsigned short, bool (Networking::*)(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&), bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);

	QueueNode<Message>* queueStartNode;
	QueueNode<Message>* holdingNode;
	QueueNode<Message>* messageOutWithMatchingIdempotencyToken;
	bool processQueue(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);
	bool processIncomingMessageQueueNode(Queue<Message>&, QueueNode<Message>*);
	static const constexpr unsigned short MAX_PROCESS_INCOMING_MESSAGE_QUEUE_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	void processIncomingError(QueueNode<Message>& msg);
	void processIncomingHeartbeat(QueueNode<Message>& msg);
	void processIncomingConfirmation(QueueNode<Message>& msg);
	char* decryptChat(Message& msg);
	void processIncomingChat(QueueNode<Message>& msg);
	void processIncomingHandshake(QueueNode<Message>& msg);

	void processIncomingMessage(QueueNode<Message>&);
	
	void (*chatMessageReceivedCallback)(char*);

	unsigned long messageResendTime(QueueNode<Message>& msg);
	bool processOutgoingMessageQueueNode(Queue<Message>&, QueueNode<Message>*);
	static const constexpr unsigned short MAX_PROCESS_OUTGOING_MESSAGE_QUEUE_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;

	bool getMessages(bool (Networking::*)(Queue<Message>&, QueueNode<Message>*), Queue<Message>&);
	static const constexpr unsigned short MAX_GET_MESSAGES_PROCESS_DURATION_MS = MAX_NETWORKING_LOOP_DURATION_MS / 3;
	unsigned short messageReceivedCount = 0;
	static const constexpr unsigned short MAX_MESSAGE_RECEIVED_COUNT = 10;

	void buildAuthenticationPayload(char*);
	void prepareOutgoingEncryptedChat(char* cipherText, unsigned short chatBytes);
	char outgoingMessageBuffer[MESSAGE_BUFFER_SIZE] = {0};
	Message& sendOutgoingMessage(Message&);

	bool exceededMaxOutgoingTokenRetryCount();
	void removeExpiredIncomingIdempotencyToken();

	//These two functions are called from doConfirmedPostProcess()
	static void removeFromQueue(Queue<Message>& fromQueue, Message& node);
	static void removeFromQueue(Networking& n, Queue<Message>& messagesOutQueue, QueueNode<Message>& messageIn, Message& messageOut);

	static void connectionEstablished(Networking& n, Queue<Message>& messagesOutQueue, QueueNode<Message>& messageIn, Message& messageOut);
public:
	Networking(unsigned long (*)(), void (*)(char*), void (*)(), const long u, bool& quit);
	~Networking();

	void processNetwork();
	void sendChatMessage(const char*);

	bool connectToPeer(IPAddress&);
};

#endif

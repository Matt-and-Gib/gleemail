#ifndef MESSAGE_H
#define MESSAGE_H

#include "Arduino.h"

#include "gleepal.h"
#include "idempotencytoken.h"
//#include "messageerror.h"


class Networking;


class Message {
private:
	const glEEpal& sender;

	MESSAGE_TYPE messageType;
	IdempotencyToken* idempotencyToken;
	const char* chat;
	const char* authentication;
	unsigned short chatLength = 0;
	//MessageError* error;

	static void noOutgoingProcess(Queue<Message>& q, Message& n) {}
	void (*outgoingPostProcess)(Queue<Message>&, Message&); //Used for removing confirmation messages once sent

	static void noConfirmedProcess(Networking& n, Queue<Message>& messagesOut, QueueNode<Message>& messageIn, Message& messageOut) {}
	void (*confirmedPostProcess)(Networking&, Queue<Message>&, QueueNode<Message>&, Message&); //Used for establishing connection and 
public:
	Message() : sender{*glEEself} {}
	
	//Incoming Message Constructor
	Message(const StaticJsonDocument<INCOMING_JSON_DOCUMENT_SIZE>& parsedDocument, const unsigned long currentTimeMS, glEEpal& from) : sender{from} {

		const unsigned short tempMessageType = parsedDocument["T"];
		messageType = static_cast<MESSAGE_TYPE>(tempMessageType);

		unsigned short tempIdempVal = parsedDocument["I"];
		idempotencyToken = new IdempotencyToken(tempIdempVal, currentTimeMS);

		const char* tempChat = parsedDocument["C"];
		chat = copyString(tempChat, MAX_MESSAGE_LENGTH);

		const char* tempAuthentication = parsedDocument["G"];
		authentication = copyString(tempAuthentication, AUTHENTICATION_PAYLOAD_SIZE);

		//error = new MessageError(parsedDocument);

		outgoingPostProcess = &noOutgoingProcess;
		confirmedPostProcess = &noConfirmedProcess;
	}

	//Outgoing Message Constructor
	Message(MESSAGE_TYPE t, IdempotencyToken* i, char* c, /*MessageError* e,*/ void (*op)(Queue<Message>&, Message&), void (*cp)(Networking&, Queue<Message>&, QueueNode<Message>&, Message&)) : sender{*glEEself} {
		messageType = t;
		idempotencyToken = i;
		chat = c;
		authentication = nullptr;
		//error = e;
		outgoingPostProcess = !op ? &noOutgoingProcess : op; //Remove conditional check by relocating noOutgoingProcess?
		confirmedPostProcess = !cp ? &noConfirmedProcess : cp; //Remove conditional check by relocating noIncomingProcess?
	}
	~Message() {
		delete idempotencyToken;
		delete[] authentication;
		delete[] chat;
		//delete error;
	}

	bool operator==(Message& o) {return (*idempotencyToken == *o.getIdempotencyToken());}

	const glEEpal& getSender() const {return sender;}
	MESSAGE_TYPE getMessageType() const {return messageType;}
	IdempotencyToken* getIdempotencyToken() {return idempotencyToken;}
	const char* getChat() {return chat;}
	const char* getAuthentication() {return authentication;}
	unsigned short getChatLength() {
		if(chatLength == 0) {
			const char* ptr = chat;
			while(*ptr++) {
				chatLength += 1;
			}
		}
		return chatLength;
	}

	//MessageError* getError() {return error;}
	void doOutgoingPostProcess(Queue<Message>& q, Message& n) {return (*outgoingPostProcess)(q, n);}
	void doConfirmedPostProcess(Networking& n, Queue<Message>& mo, QueueNode<Message>& messageIn) {(*confirmedPostProcess)(n, mo, messageIn, *this);}
};

#endif
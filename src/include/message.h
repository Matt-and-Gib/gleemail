#ifndef MESSAGE_H
#define MESSAGE_H

#include "Arduino.h"

#include "gleepal.h"
#include "idempotencytoken.h"
//#include "messageerror.h"


class Networking;


class Message {
private:
	glEEpal& sender = *glEEself;

	MESSAGE_TYPE messageType;
	IdempotencyToken* idempotencyToken;
	const char* chat;
	//MessageError* error;

	static void noOutgoingProcess(Queue<Message>& q, Message& n) {}
	void (*outgoingPostProcess)(Queue<Message>&, Message&); //Used for removing confirmation messages once sent

	static void noConfirmedProcess(Networking& n, Queue<Message>& messagesOut, QueueNode<Message>& messageIn, Message& messageOut) {}
	void (*confirmedPostProcess)(Networking&, Queue<Message>&, QueueNode<Message>&, Message&); //Used for establishing connection and 
public:
	Message() {}
	
	//Incoming Message Constructor
	Message(const StaticJsonDocument<INCOMING_JSON_DOCUMENT_SIZE>& parsedDocument, const unsigned long currentTimeMS, glEEpal& from) {
		Serial.println(F("1"));

		const unsigned short tempMessageType = parsedDocument["T"];
		messageType = static_cast<MESSAGE_TYPE>(tempMessageType);

		Serial.println(F("2"));

		unsigned short tempIdempVal = parsedDocument["I"];
		idempotencyToken = new IdempotencyToken(tempIdempVal, currentTimeMS);

		Serial.println(F("3"));

		const char* tempChat = parsedDocument["C"];
		chat = copyString(tempChat, MAX_MESSAGE_LENGTH);
		//error = new MessageError(parsedDocument);

		Serial.println(F("4"));

		outgoingPostProcess = &noOutgoingProcess;
		confirmedPostProcess = &noConfirmedProcess;

		Serial.println(F("5"));

		sender = from;

		Serial.println(F("6"));
	}

	//Outgoing Message Constructor
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
	MESSAGE_TYPE getMessageType() const {return messageType;}
	IdempotencyToken* getIdempotencyToken() {return idempotencyToken;}
	const char* getChat() {return chat;}
	//MessageError* getError() {return error;}
	void doOutgoingPostProcess(Queue<Message>& q, Message& n) {return (*outgoingPostProcess)(q, n);}
	void doConfirmedPostProcess(Networking& n, Queue<Message>& mo, QueueNode<Message>& messageIn) {(*confirmedPostProcess)(n, mo, messageIn, *this);}
};

#endif
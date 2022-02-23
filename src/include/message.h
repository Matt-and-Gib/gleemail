#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef UNIT_TEST_MODE
//include moc_arduino.h
//include moc_ArduinoJson.hpp
//include moc_gleepal.h
//include moc_idempotencytoken.h
//include moc_messageerror.h
#else
#include "Arduino.h"
#include <ArduinoJson.hpp>
#include "gleepal.h"
#include "idempotencytoken.h"
//#include "messageerror.h"
#endif


class Networking;


class Message final {
private:
	const glEEpal& sender;

	MESSAGE_TYPE messageType;
	IdempotencyToken* idempotencyToken;
	const unsigned char* chat;
	const unsigned char* authentication;
	unsigned short chatLength = 0;
	//MessageError* error;

	static void noOutgoingProcess(Queue<Message>&, Message&) {}
	void (*outgoingPostProcess)(Queue<Message>&, Message&); //Used for removing confirmation messages once sent

	static void noConfirmedProcess(Networking&, Queue<Message>&, QueueNode<Message>&, Message&) {}
	void (*confirmedPostProcess)(Networking&, Queue<Message>&, QueueNode<Message>&, Message&); //Used for establishing connection and 
public:
	explicit Message() : sender{*glEEself} {}
	
	//Incoming Message Constructor
	explicit Message(const ArduinoJson::StaticJsonDocument<INCOMING_JSON_DOCUMENT_SIZE>& parsedDocument, const unsigned long currentTimeMS, glEEpal& from) : sender{from} {

		const unsigned short tempMessageType = parsedDocument["T"];
		messageType = static_cast<MESSAGE_TYPE>(tempMessageType);

		unsigned short tempIdempVal = parsedDocument["I"];
		idempotencyToken = new IdempotencyToken(tempIdempVal, currentTimeMS);

		const char* tempChat = parsedDocument["C"];
		chat = copyString<const char, unsigned char>(tempChat, MAX_MESSAGE_LENGTH); //NOTE: Now that max message length has been increased, this is a less-trivial copy.

		const char* tempAuthentication = parsedDocument["G"];
		authentication = copyString<const char, unsigned char>(tempAuthentication, AUTHENTICATION_PAYLOAD_SIZE);

		//error = new MessageError(parsedDocument);

		outgoingPostProcess = &noOutgoingProcess;
		confirmedPostProcess = &noConfirmedProcess;
	}

	//Outgoing Message Constructor
	explicit Message(MESSAGE_TYPE t, IdempotencyToken* i, unsigned char* c, /*MessageError* e,*/ void (*op)(Queue<Message>&, Message&), void (*cp)(Networking&, Queue<Message>&, QueueNode<Message>&, Message&)) : sender{*glEEself} {
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

	bool operator==(const Message& o) const {return (*idempotencyToken == *(o.getIdempotencyToken()));}

	const glEEpal& getSender() const {return sender;}
	MESSAGE_TYPE getMessageType() const {return messageType;}
	IdempotencyToken* getIdempotencyToken() const {return idempotencyToken;}
	const unsigned char* getChat() const {return chat;}
	const unsigned char* getAuthentication() const {return authentication;}
	unsigned short getChatLength() {
		if(chatLength == 0) {
			const unsigned char* ptr = chat;
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
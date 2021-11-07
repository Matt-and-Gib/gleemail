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
	short chatLength = -1;
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
		//error = new MessageError(parsedDocument);

		outgoingPostProcess = &noOutgoingProcess;
		confirmedPostProcess = &noConfirmedProcess;
	}

	//Outgoing Message Constructor
	Message(MESSAGE_TYPE t, IdempotencyToken* i, char* c, /*MessageError* e,*/ void (*op)(Queue<Message>&, Message&), void (*cp)(Networking&, Queue<Message>&, QueueNode<Message>&, Message&)) : sender{*glEEself} {
		messageType = t;
		idempotencyToken = i;
		chat = c;
		//error = e;
		outgoingPostProcess = !op ? &noOutgoingProcess : op; //Remove conditional check by relocating noOutgoingProcess?
		confirmedPostProcess = !cp ? &noConfirmedProcess : cp; //Remove conditional check by relocating noIncomingProcess?
	}
	~Message() {
		Serial.println(F("before delete idempotency token"));
		delete idempotencyToken;
		Serial.println(F("before delete chat"));
		delete[] chat;
		Serial.println(F("after delete chat"));
		//delete error;
	}

	bool operator==(Message& o) {return (*idempotencyToken == *o.getIdempotencyToken());}

	const glEEpal& getSender() const {return sender;}
	MESSAGE_TYPE getMessageType() const {return messageType;}
	IdempotencyToken* getIdempotencyToken() {return idempotencyToken;}
	const char* getChat() {return chat;}
	unsigned short getChatLength() {
		if(chatLength < 0) {
			unsigned short i = 0;
			while(chat[i] != '\0') {
				i += 1;
			}
			chatLength = i;

			//while(chat[++chatLength] != '\0') {}

			/*while(*chat++) {
				chatLength += 1;
			}*/
		}

		Serial.print(F("chat length: "));
		Serial.println(chatLength);

		Serial.print(F("chat over chat length:"));
		for(unsigned short i = 0; i < chatLength; i += 1) {
			Serial.print(' ');
			Serial.print(chat[i], HEX);
		}
		Serial.println();

		return chatLength;
	}

	//MessageError* getError() {return error;}
	void doOutgoingPostProcess(Queue<Message>& q, Message& n) {return (*outgoingPostProcess)(q, n);}
	void doConfirmedPostProcess(Networking& n, Queue<Message>& mo, QueueNode<Message>& messageIn) {(*confirmedPostProcess)(n, mo, messageIn, *this);}
};

#endif
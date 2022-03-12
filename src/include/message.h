#ifndef MESSAGE_H
#define MESSAGE_H

#ifdef UNIT_TEST_MODE
//include moc_arduino.h
//include moc_ArduinoJson.hpp
//include moc_gleepal.h
//include moc_idempotencytoken.h
#else
#include "Arduino.h"
#include <ArduinoJson.hpp>
#include "gleepal.h"
#include "idempotencytoken.h"
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

	static void noOutgoingProcess(Queue<Message>&, Message&) {}
	void (*outgoingPostProcess)(Queue<Message>&, Message&); //Used for removing confirmation messages once sent

	static void noConfirmedProcess(Networking&, Queue<Message>&, QueueNode<Message>&, Message&) {}
	void (*confirmedPostProcess)(Networking&, Queue<Message>&, QueueNode<Message>&, Message&); //Used for establishing connection and 
public:
	Message() :
		sender{*glEEself},
		messageType{MESSAGE_TYPE::NONE},
		idempotencyToken{0},
		chat{nullptr},
		authentication{nullptr},
		outgoingPostProcess{noOutgoingProcess},
		confirmedPostProcess{noConfirmedProcess}
	{}
	Message(const Message&) = delete;
	Message(Message&&) = delete;
	Message& operator=(const Message&) = delete;
	Message& operator=(Message&&) = delete;
	
	//Incoming Message Constructor
	Message(
		const ArduinoJson::StaticJsonDocument<INCOMING_JSON_DOCUMENT_SIZE>& parsedDocument,
		const unsigned long currentTimeMS,
		glEEpal& from
	) :
		sender{from}
	{
		const unsigned short tempMessageType = parsedDocument["T"];
		messageType = static_cast<MESSAGE_TYPE>(tempMessageType);

		unsigned short tempIdempVal = parsedDocument["I"];
		idempotencyToken = new IdempotencyToken(tempIdempVal, currentTimeMS);

		const char* tempChat = parsedDocument["C"];
		chat = copyString<const char, unsigned char>(tempChat, MAX_MESSAGE_LENGTH); //NOTE: Now that max message length has been increased, this is a less-trivial copy.

		const char* tempAuthentication = parsedDocument["G"];
		authentication = copyString<const char, unsigned char>(tempAuthentication, AUTHENTICATION_PAYLOAD_SIZE);

		outgoingPostProcess = &noOutgoingProcess;
		confirmedPostProcess = &noConfirmedProcess;
	}

	//Outgoing Message Constructor
	Message(
		MESSAGE_TYPE t,
		IdempotencyToken* const i,
		unsigned char* const c,
		void (* const op)(Queue<Message>&, Message&),
		void (* const cp)(Networking&, Queue<Message>&,
		QueueNode<Message>&, Message&)
	) :
		sender{*glEEself},
		messageType{t},
		idempotencyToken{i},
		chat{c},
		authentication{nullptr}
	{
		outgoingPostProcess = !op ? &noOutgoingProcess : op; //Remove conditional check by relocating noOutgoingProcess?
		confirmedPostProcess = !cp ? &noConfirmedProcess : cp; //Remove conditional check by relocating noIncomingProcess?
	}
	
	~Message() {
		delete idempotencyToken;
		delete[] authentication;
		delete[] chat;
	}

	bool operator==(const Message& o) const {return (*idempotencyToken == *(o.getIdempotencyToken()));}

	[[nodiscard]] const glEEpal& getSender() const {return sender;}
	[[nodiscard]] MESSAGE_TYPE getMessageType() const {return messageType;}
	[[nodiscard]] IdempotencyToken* getIdempotencyToken() const {return idempotencyToken;}
	[[nodiscard]] const unsigned char* getChat() const {return chat;}
	[[nodiscard]] const unsigned char* getAuthentication() const {return authentication;}
	[[nodiscard]] unsigned short getChatLength() {
		if(chatLength == 0) {
			const unsigned char* ptr = chat;
			while(*ptr++) {
				chatLength += 1;
			}
		}
		return chatLength;
	}

	void doOutgoingPostProcess(Queue<Message>& q, Message& n) {return (*outgoingPostProcess)(q, n);}
	void doConfirmedPostProcess(Networking& n, Queue<Message>& mo, QueueNode<Message>& messageIn) {(*confirmedPostProcess)(n, mo, messageIn, *this);}
};

#endif
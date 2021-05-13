#include "Arduino.h"
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


void basicTest() {
	IdempotencyToken* orig = new IdempotencyToken(117, millis());
	IdempotencyToken* copy = new IdempotencyToken(*orig);
	Serial.println("Tokens with valid values constructed...");

	delete copy;
	Serial.println("valid value copy deleted!");
	delete orig;
	Serial.println("valid value orig deleted!");


	orig = new IdempotencyToken(NULL, NULL);
	copy = new IdempotencyToken(*orig);
	Serial.println("Tokens with invalid values constructed...");

	delete copy;
	Serial.println("invalid value copy deleted!");
	delete orig;
	Serial.println("invalid value orig deleted!");
}


struct TestObj {
	IdempotencyToken* idtk = new IdempotencyToken(1337, 511);
	IdempotencyToken* getIdempotencyToken() {return idtk;}
};


void advTest() {
	TestObj* myObj = new TestObj();
	Queue<IdempotencyToken> messagesInIdempotencyTokens = *new Queue<IdempotencyToken>;
	QueueNode<TestObj> msg = *new QueueNode<TestObj>(myObj);

	messagesInIdempotencyTokens.enqueue(new IdempotencyToken(*(msg.getData()->getIdempotencyToken())));

	Serial.println("Created and enqueued objects");

	QueueNode<IdempotencyToken>* nextTokenNode = messagesInIdempotencyTokens.peek();
	messagesInIdempotencyTokens.dequeue();
	//delete nextTokenNode;
	//Serial.println("Deleted nextTokenNode");
	
	delete &msg; //ERROR HERE: probably something to do with taking address?
	Serial.println("Deleted msg");

	//delete &messagesInIdempotencyTokens;
	//Serial.println("Deleted messagesInIdempotencyTokens");

	//delete myObj;
	//Serial.println("Deleted myObj");

	Serial.println("All deletions complete");
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	Serial.println("Start");

	basicTest();
	Serial.println("Basic Test Done");

	advTest();
	Serial.println("Advanced Test Done");

	Serial.println("Done");
}


void loop() {

}
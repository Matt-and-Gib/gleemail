#include "Arduino.h"
#include "HardwareSerial.h"

#include "queue.h"


class Swimmer {
private:
	const char* name;
public:
	Swimmer(const char* title) {
		name = title;
	}
	~Swimmer() {
		delete[] name;
	}

	const char* getName() const {return name;}

	bool operator==(const Swimmer& o) {return name[0] == o.name[0];}
};


void loop() {

}


void basicTest() {
	Queue<Swimmer> pool;

	Swimmer* mp = new Swimmer("Michael Phelps");

	pool.enqueue(new Swimmer("Eric"));
	pool.enqueue(mp);
	pool.enqueue(new Swimmer("The cat named Molly"));

	QueueNode<Swimmer>* currentNode = pool.dequeue();
	while(currentNode) {
		Serial.println(currentNode->getData()->getName());

		delete currentNode;
		currentNode = pool.dequeue();
	}
}


void advancedTest() {
	Queue<Swimmer> pool;

	Swimmer* mp = new Swimmer("Michael Phelps");

	pool.enqueue(new Swimmer("Eric"));
	pool.enqueue(mp);
	pool.enqueue(new Swimmer("The cat named Molly"));

	delete pool.remove(*mp);

	QueueNode<Swimmer>* currentNode = pool.dequeue();
	while(currentNode) {
		Serial.println(currentNode->getData()->getName());

		delete currentNode;
		currentNode = pool.dequeue();
	}
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	basicTest();
	Serial.println("Done\n");

	advancedTest();
	Serial.println("Done\n");
}
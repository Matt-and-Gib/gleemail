#include "Arduino.h"
#include "HardwareSerial.h"

#include "linkedlist.h"


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
};


void loop() {

}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	LinkedList<Swimmer> pool;

	pool.enqueue(new Swimmer("Eric"));
	pool.enqueue(new Swimmer("Michael Phelps"));
	pool.enqueue(new Swimmer("The cat named Molly"));

	LinkedListNode<Swimmer>* currentNode = pool.dequeue();
	while(currentNode) {
		Serial.println(currentNode->getData()->getName());
		currentNode = pool.dequeue();
	}
}
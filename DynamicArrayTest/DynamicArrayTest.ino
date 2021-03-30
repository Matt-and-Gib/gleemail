#include "Arduino.h"

#include "dynamicarray.h"


struct MyPair {
	int key;
	char val;

	MyPair() {
		key = 0;
		val = '\0';
	}

	MyPair(int k, char v) {
		key = k;
		val = v;
	}
};


void basicTests() {
	DynamicArray<MyPair> arr;

	Serial.print("array is of size ");
	Serial.print(arr.getSize());
	Serial.print(" and contains ");
	Serial.print(arr.getLength());
	Serial.println(" items.");

	for(int i = 0; i < 17; i += 1) {
		arr.push(new MyPair(i, (i%2 == 0 ? 't' : 'f')));
	}

	Serial.print("array is of size ");
	Serial.print(arr.getSize());
	Serial.print(" and contains ");
	Serial.print(arr.getLength());
	Serial.println(" items.");

	arr.removeAtIndex(17);
	arr.removeAtIndex(16);

	Serial.print("array is of size ");
	Serial.print(arr.getSize());
	Serial.print(" and contains ");
	Serial.print(arr.getLength());
	Serial.println(" items.");

	Serial.print("value at 4 is ");
	Serial.println(arr.getAtIndex(4)->val);
	Serial.print("value at 5 is ");
	Serial.println(arr.getAtIndex(5)->val);

	Serial.print("the first value is ");
	Serial.println(arr.getFirst()->val);

	Serial.print("the last value is ");
	Serial.println(arr.getFirst()->val);

	arr.pop();
	arr.pop();
	arr.pop();

	Serial.print("array is of size ");
	Serial.print(arr.getSize());
	Serial.print(" and contains ");
	Serial.print(arr.getLength());
	Serial.println(" items.");
}


void advancedTests() {

}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	Serial.println("Begin basic tests");
	basicTests();
	Serial.println("Basic tests complete");

	advancedTests();
}


void loop() {

}
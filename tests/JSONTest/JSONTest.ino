#include "Arduino.h"
#include "ArduinoJson.h"


/*
{
  "sensor": "gps",
  "time": 1351824120,
  "data": [
    48.75608,
    2.302038
  ]
}


{"sensor":"gps","time":1351824120,"data":[48.75608,2.302038]}
*/


/*
Start
Begin parsing
Done parsing
sensor: gps
time: 1351824120
fake val: 0
data_0: 48.76
data_1: 2.30
Done

*/

/*
{
  "v": [
    "a",
    "b",
    "c"
  ]
}
*/

void testParseMissingData() {
	char output[32];

	StaticJsonDocument<32> doc;

	//char goblinblabla[3] = {0x41, 0x00, 0x43};
	char goblinblabla[3] = {41, 0, 43};

	//41,0,43

	//doc["v"] = "ABC";
	doc["v"] = goblinblabla; //{'A', 'B', 'C'};

	serializeJson(doc, output);

	Serial.print(F("Serialized: "));
	Serial.println(output);


/*
Start
Serialized: {"v":"A"}
V: A
V (goblin-speak): 4103A
Done
*/

/*
m = {0x12, 0x33, 0x54};
ms = "120054";
*/


	const constexpr unsigned short MAX_INPUT_LENGTH = 12;

	//								{		"	v		"	:		"	A		\0	C		"	}		\0
	char input[MAX_INPUT_LENGTH] = {0x7b, 0x22, 0x76, 0x22, 0x3a, 0x22, 0x41, 0x00, 0x43, 0x22, 0x7d, 0x00};
	//char input[MAX_INPUT_LENGTH] = "{\"v\":\"ABC\"}";

	//StaticJsonDocument<16> doc;

	DeserializationError error = deserializeJson(doc, output, MAX_INPUT_LENGTH);

	if(error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

	const char* value = doc["v"];

	Serial.print(F("V: "));
	for(unsigned short i = 0; i < 3; i += 1) {
		Serial.print(value[i]);
	}
	Serial.println();

	Serial.print(F("V (goblin-speak): "));
	for(unsigned short i = 0; i < 3; i += 1) {
		Serial.print(value[i], HEX);
	}
	Serial.println();

	/*
	Start
	V: ABC
	V (goblin-speak): 41 42 43
	Done

	*/


	/*char message[20] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	//char serializedJsonFromRouter[96] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.75608,2.302038]}";
	StaticJsonDocument<20> doc;

//	DeserializationError error = deserializeJson(doc, serializedJsonFromRouter, 20);
	DeserializationError error = deserializeJson(doc, message, 20);

	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

	Serial.println("Begin parsing");

	const char* sensor = doc["sensor"]; // "gps"
	long time = doc["time"]; // 1351824120
	bool fakeVal = doc["fake"];

	float data_0 = doc["data"][0]; // 48.75608
	float data_1 = doc["data"][1]; // 2.302038

	Serial.println("Done parsing");

	Serial.print("sensor: ");
	Serial.println(sensor);

	Serial.print("time: ");
	Serial.println(time);

	Serial.print("fake val: ");
	Serial.println(fakeVal);

	Serial.print("data_0: ");
	Serial.println(data_0);

	Serial.print("data_1: ");
	Serial.println(data_1);*/
}


void testSerializeWorstCaseObjectSize() {
	char output[512];

	StaticJsonDocument<512> doc;

	doc["T"] = 4;
	doc["I"] = 65535;
	doc["C"] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	doc["G"] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

	serializeJson(doc, output);

	Serial.print(F("size: "));
	Serial.println(measureJson(doc));
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	Serial.println("Start");
	//testParseMissingData();
	testSerializeWorstCaseObjectSize();
	Serial.println("Done");
}


void loop() {
	
}
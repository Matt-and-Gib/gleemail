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


void testParseMissingData() {
	char serializedJsonFromRouter[96] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.75608,2.302038]}";
	StaticJsonDocument<96> doc;

	DeserializationError error = deserializeJson(doc, serializedJsonFromRouter, 96);

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
	Serial.println(data_1);
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	Serial.println("Start");
	testParseMissingData();
	Serial.println("Done");
}


void loop() {
	
}
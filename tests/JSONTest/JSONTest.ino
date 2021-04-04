#include "Arduino.h"
#include <ArduinoJSON.h>

// The documentation that I'm referencing is from
// https://github.com/bblanchon/ArduinoJson https://arduinojson.org/v6/assistant/


/*
TODO
	[]		1. download JSON
	[v]		2. deserialize JSON
	[v]		3. create tree from JSON
*/


//using namespace ArduinoJson;


class MoreTaze {
public:
	MoreTaze() {}
	MoreTaze(const char* sl) {
		tazeChairs = new char[6];

		int i = 0;
		char c = sl[i];
		while (c != '\0') {
			tazeChairs[i] = c;
			c = sl[++i];
		}
	}
	char* tazeChairs;
};


class ChairPear {
public:
	ChairPear() {}
	ChairPear(const char* l, MoreTaze& p) {
		letter = l;
		phrase = p;
	}

	const char* letter;
	MoreTaze phrase;
};


void basicTest() {
	const char* pretendWeJustDownloadedThis ="{\"morsecodetreedata\": [{\"phrase\": \".\", \"char\": \"E\"},{\"phrase\": \"-\", \"char\": \"T\"}]}";

	static constexpr unsigned short CALCULATED_DOCUMENT_SIZE_IN_BYTES = 3424;
	ArduinoJson::DynamicJsonDocument doc(CALCULATED_DOCUMENT_SIZE_IN_BYTES);
	ArduinoJson::DeserializationError error = deserializeJson(doc, pretendWeJustDownloadedThis);

	if (error) {
		Serial.print(F("deserializeJson() failed: "));
		Serial.println(error.f_str());
		return;
	}

	int treeIndex = 0;
	ChairPear* pearTree = new ChairPear[2];
	const char* l; //variable pointer to const char
	const char* p; //variable pointer to const char

	for (ArduinoJson::JsonObject elem : doc["morsecodetreedata"].as<ArduinoJson::JsonArray>()) {
		l = elem["char"];
		p = elem["phrase"];
		pearTree[treeIndex++] = *new ChairPear(l, *new MoreTaze(p));

	}

	Serial.println(pearTree[0].letter);
	Serial.println(pearTree[0].phrase.tazeChairs[0]);

	Serial.println(pearTree[1].letter);
	Serial.println(pearTree[1].phrase.tazeChairs[0]);

	Serial.println(pearTree[0].letter);
	Serial.println(pearTree[0].phrase.tazeChairs[0]);

	Serial.println(pearTree[1].letter);
	Serial.println(pearTree[1].phrase.tazeChairs[0]);
}

void setup() {
	Serial.begin(9600);
	while (!Serial) delay(250);

	Serial.println("Begin basic tests");
	basicTest();
	Serial.println("End basic tests");
}

void loop() {}
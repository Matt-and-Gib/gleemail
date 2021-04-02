#include "Arduino.h"
#include <SPI.h>
#include <WiFiNINA.h>

static constexpr char SSID[] = "CHANGE_ME";
static constexpr char PASSWORD[] = "CHANGE_ME";
static char* jsonData;


void setup () {
	Serial.begin(9600);
	while(!Serial) delay(250);

	int status = WL_IDLE_STATUS;
	while(status != WL_CONNECTED) {
		Serial.print("Attempting to connect to ");
		Serial.print(SSID);
		Serial.println("...");

		status = WiFi.begin(SSID, PASSWORD);

		delay(3000);
	}
	Serial.println("Connected to network!");

	static constexpr char server[] = "raw.githubusercontent.com";
	WiFiClient client;

	Serial.println("Attempting to download Morse Code JSON...");

	if(!client.connectSSL(server, 443)) {
		Serial.println("Unable to connect to server.");
		Serial.flush();
		abort();
	}

	Serial.println("Requesting web data...");
	client.println("GET /Matt-and-Gib/gleemail/main/MorseCodeCharPairs.json HTTP/1.1");
	client.println("User-Agent: ArduinoWifiClient");
	client.println("Host: raw.githubusercontent.com");
	client.println("Accept: */*");
	client.println("Connection: close");
	client.println();

	//adjust based on latency?
	delay(2000);

	static constexpr short DATA_BUFFER_SIZE = 4096;
	int bufferIndex = 0;
	char dataBuffer[DATA_BUFFER_SIZE];

	while(client.available()) {
		if(bufferIndex < DATA_BUFFER_SIZE) {
			dataBuffer[bufferIndex++] = client.read();
		} else {
			Serial.println("Data buffer overflow!"); //lol how big are these headers
			break;
		}
	}

	Serial.print("Used ");
	Serial.print(bufferIndex);
	Serial.print(" out of max ");
	Serial.println(DATA_BUFFER_SIZE);

	//Consider implementing Knuth Morris Pratt algorithm
	//Note: header ends with "\r\n\r\n" according to section 4.1 of RFC 2616: https://tools.ietf.org/html/rfc2616#section-4.1

	static constexpr char HEADER_END_STRING[] = "\r\n\r\n";
	static constexpr unsigned short LENGTH_OF_HEADER_END_STRING = sizeof(HEADER_END_STRING)/sizeof(HEADER_END_STRING[0]) - 1;
	unsigned short headerEndSearchIndex = 0;
	short endOfHeaderIndex = -1;

	for(unsigned short beginningSubstringIndex = 0; beginningSubstringIndex < bufferIndex; beginningSubstringIndex += 1) {
		if(dataBuffer[beginningSubstringIndex] == '\r') {
			headerEndSearchIndex = 0;
			for(unsigned short endSubstringIndex = 0; endSubstringIndex < LENGTH_OF_HEADER_END_STRING; endSubstringIndex += 1) {
				if(dataBuffer[beginningSubstringIndex + endSubstringIndex] != HEADER_END_STRING[headerEndSearchIndex++]) {
					goto headerEndMismatch; //should probably return an index here instead, but this is all one function in prototype
				}
			}

			endOfHeaderIndex = beginningSubstringIndex + LENGTH_OF_HEADER_END_STRING;
		}
		headerEndMismatch:;
	}

	if(endOfHeaderIndex != -1) {
		const unsigned short LENGTH_OF_JSON_BODY = bufferIndex - endOfHeaderIndex;
		jsonData = new char[LENGTH_OF_JSON_BODY];
		for(int i = 0; i < LENGTH_OF_JSON_BODY; i += 1) {
			jsonData[i] = dataBuffer[endOfHeaderIndex + i];
		}

		Serial.println("JSON Payload:\n");
		for(int i = 0; i < LENGTH_OF_JSON_BODY; i += 1) {
			Serial.print(jsonData[i]);
		}
		Serial.println("\nDone");
	} else {
		Serial.println("Did not find end of header! Full body:");
		Serial.println("Got:\n");
		for(int i = 0; i < bufferIndex; i += 1) {
			Serial.print(dataBuffer[i]);
		}
		Serial.println("\nDone");
	}

	if(!client.connected()) {
		Serial.println("Disconnecting from server...");
		client.stop();
	}
}


void loop() {

}
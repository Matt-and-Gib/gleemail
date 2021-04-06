#include "Arduino.h"
#include "HardwareSerial.h"
#include <WiFiNINA.h>
#include <WiFiUdp.h>


IPAddress localIPAddress(192,168,1,110);
const unsigned short localPort = 1900;

const IPAddress friendsIPAddress(75,72,145,98);
const unsigned short friendsPort = 1900;

WiFiUDP Udp;
char sendBuffer[] = "Got your message!";
char *receiveBuffer = new char[256];

unsigned int packetSize = 0;


bool connectToWifi() {
	unsigned short inputLength = 0;

	Serial.println("Enter WiFi SSID:");
	char userSSID[33];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userSSID, 33);
			break;
		}

		delay(250);
	}
	userSSID[inputLength] = '\0';

	char userPassword[64];
	Serial.print("Enter password for ");
	Serial.print(userSSID);
	Serial.println(":");
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userPassword, 64);
			break;
		}

		delay(250);
	}
	userPassword[inputLength] = '\0';

	Serial.println("Attempting connection...");

	if(userSSID == nullptr || userPassword == nullptr) {
		Serial.println("Invalid SSID or Password");
		return false;
	}

	WiFi.begin(userSSID, userPassword);
	while(WiFi.status() == WL_IDLE_STATUS) {
		delay(250);
	}

	if(WiFi.status() == WL_CONNECTED) {
		return true;
	} else {
		Serial.println("Network connection failed");
		return false;
	}
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	if(!connectToWifi()) {
		Serial.println("Error!");
		abort();
	}

	Udp.begin(localPort);

	Serial.println("Ready!");

	if(WiFi.localIP() == localIPAddress) {
		Serial.println("Sending first message");
		Udp.beginPacket(friendsIPAddress, friendsPort);
		Udp.write(sendBuffer);
		Udp.endPacket();
	}
}


void loop() {
	if(Udp.parsePacket()) {
		Serial.print("Received message from ");
		Serial.print(Udp.remoteIP());
		Serial.print(" on port ");
		Serial.print(Udp.remotePort());
		Serial.print(": ");

		packetSize = Udp.read(receiveBuffer, 255);
		receiveBuffer[packetSize] = '\0';
		Serial.println(receiveBuffer);

		delay(2000);

		Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
		Udp.write(sendBuffer);
		Udp.endPacket();
	}
}
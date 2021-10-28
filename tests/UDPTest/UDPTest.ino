
/*

  WiFi UDP Send and Receive String

 This sketch wait an UDP packet on localPort using the WiFi module.

 When a packet is received an Acknowledge packet is sent to the client on port remotePort

 created 30 December 2012

 by dlf (Metodo2 srl)

 */

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

int status = WL_IDLE_STATUS;
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "CHANGE ME";    // your network SSID (name)
char pass[] = "CHANGE ME";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

unsigned int localPort = 2390;      // local port to listen on

char packetBuffer[256]; //buffer to hold incoming packet
char ReplyBuffer[] = "acknowledged";       // a string to send back

const unsigned short SENDLEN = 4;
char sendMessage[SENDLEN] = {0x41, 0x42, 0x43, 0x00}; //ABC\0

WiFiUDP Udp;


void sendMessageGOBLIN(const IPAddress sendToIP) {
	Udp.beginPacket(sendToIP, 29453);
	Udp.write(sendMessage, SENDLEN);
	Udp.endPacket();

	Serial.println(F("sent!"));
}


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
	; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
	Serial.println("Communication with WiFi module failed!");
	// don't continue
	while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
	Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
	Serial.print("Attempting to connect to SSID: ");
	Serial.println(ssid);
	// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
	status = WiFi.begin(ssid, pass);
	// wait 10 seconds for connection:
	delay(10000);
  }

  Serial.println("Connected to wifi");
  Serial.println("\nStarting connection to server...");

  // if you get a connection, report back via serial:
  Udp.begin(localPort);

	IPAddress sendToPeerIP = {192, 168, 0, 1};
	sendMessageGOBLIN(sendToPeerIP);
}

void loop() {
	if(millis() % 1000 == 0) {
		sendMessageGOBLIN(Udp.remoteIP());
	}
	
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
	Serial.print("Received packet of size ");
	Serial.println(packetSize);
	Serial.print("From ");
	IPAddress remoteIp = Udp.remoteIP();
	Serial.print(remoteIp);
	Serial.print(", port ");
	Serial.println(Udp.remotePort());

	// read the packet into packetBufffer
	int len = Udp.read(packetBuffer, 255);
	if (len > 0) {
	  packetBuffer[len] = 0;
	}

	Serial.println("Contents:");
	Serial.println(packetBuffer); //change (print hex)

	sendMessageGOBLIN(Udp.remoteIP());
  }
}
#include <SD.h>
#include <SPI.h>


const int chipSelect = 10;

void setup() {
	Serial.begin(9600);
	while (!Serial) {
		delay(250);
	}

	Serial.print("Initializing SD card...");

	if(!SD.begin(chipSelect)) {
		Serial.println("Card failed, or not present");
		abort();
	}

	Serial.println("card initialized.");

	File dataFile = SD.open("GLEEMAIL/PREFS.GMD");
	if(dataFile) {
		while(dataFile.available()) {
			Serial.write(dataFile.read());
		}

		dataFile.close();
	} else {
		dataFile.close();
		Serial.println("error opening datalog.txt");
	}
}

void loop() {}
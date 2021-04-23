#include <Arduino.h>
#include <HardwareSerial.h>

#include <stdint.h>

#include "src/tiny-AES-c-1.0.0/aes.hpp"


void loop() {}


void printArr(const uint8_t* const arr, const unsigned short len, const bool cast = true) {
	for(unsigned short i = 0; i < len; i += 1) {
		if(cast) {
			Serial.print((char)arr[i]);
		} else {
			Serial.print(arr[i]);
		}
	}
	Serial.println();
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

#ifndef AES128
	Serial.print("Specify AES128");
	return;
#endif

	uint8_t key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	uint8_t initVector[] = "iviviviviviviviv";
	struct AES_ctx ctx;

	unsigned char message[] = "{\n\t\"morsecodetreedata\": [\n\t\t{\"phrase\": \".\", \"symbol\": \"E\"},\n\t\t{\"phrase\": \"-\", \"symbol\": \"T\"},\n\n\t\t{\"phrase\": \"..\", \"symbol\": \"I\"},\n\t\t{\"phrase\": \".-\", \"symbol\": \"A\"},\n\t\t{\"phrase\": \"-.\", \"symbol\": \"N\"},\n\t\t{\"phrase\": \"--\", \"symbol\": \"M\"},\n\n\t\t{\"phrase\": \"...\", \"symbol\": \"S\"},\n\t\t{\"phrase\": \"..-\", \"symbol\": \"U\"},\n\t\t{\"phrase\": \".-.\", \"symbol\": \"R\"},\n\t\t{\"phrase\": \".--\", \"symbol\": \"W\"},\n\t\t{\"phrase\": \"-..\", \"symbol\": \"D\"},\n\t\t{\"phrase\": \"-.-\", \"symbol\": \"K\"},\n\t\t{\"phrase\": \"--.\", \"symbol\": \"G\"},\n\t\t{\"phrase\": \"---\", \"symbol\": \"O\"},\n\n\t\t{\"phrase\": \"....\", \"symbol\": \"H\"},\n\t\t{\"phrase\": \"...-\", \"symbol\": \"V\"},\n\t\t{\"phrase\": \"..-.\", \"symbol\": \"F\"},\n\t\t{\"phrase\": \"..--\", \"symbol\": \" \"},\n\t\t{\"phrase\": \".-..\", \"symbol\": \"L\"},\n\t\t{\"phrase\": \".-.-\", \"symbol\": \" \"},\n\t\t{\"phrase\": \".--.\", \"symbol\": \"P\"},\n\t\t{\"phrase\": \".---\", \"symbol\": \"J\"},\n\t\t{\"phrase\": \"-...\", \"symbol\": \"B\"},\n\t\t{\"phrase\": \"-..-\", \"symbol\": \"X\"},\n\t\t{\"phrase\": \"-.-.\", \"symbol\": \"C\"},\n\t\t{\"phrase\": \"-.--\", \"symbol\": \"Y\"},\n\t\t{\"phrase\": \"--..\", \"symbol\": \"Z\"},\n\t\t{\"phrase\": \"--.-\", \"symbol\": \"Q\"},\n\t\t{\"phrase\": \"---.\", \"symbol\": \" \"},\n\t\t{\"phrase\": \"----\", \"symbol\": \" \"},\n\n\t\t{\"phrase\": \".....\", \"symbol\": \"5\"},\n\t\t{\"phrase\": \"....-\", \"symbol\": \"4\"},\n\t\t{\"phrase\": \"...--\", \"symbol\": \"3\"},\n\t\t{\"phrase\": \"..--.\", \"symbol\": \" \"},\n\t\t{\"phrase\": \"..---\", \"symbol\": \"2\"},\n\t\t{\"phrase\": \".-...\", \"symbol\": \"&\"},\n\t\t{\"phrase\": \".-..-\", \"symbol\": \" \"},\n\t\t{\"phrase\": \".-.-.\", \"symbol\": \"+\"},\n\t\t{\"phrase\": \".--.-\", \"symbol\": \" \"},\n\t\t{\"phrase\": \".----\", \"symbol\": \"1\"},\n\t\t{\"phrase\": \"-....\", \"symbol\": \"6\"},\n\t\t{\"phrase\": \"-...-\", \"symbol\": \"=\"},\n\t\t{\"phrase\": \"-..-.\", \"symbol\": \"/\"},\n\t\t{\"phrase\": \"-.--.\", \"symbol\": \"(\"},\n\t\t{\"phrase\": \"--...\", \"symbol\": \"7\"},\n\t\t{\"phrase\": \"--..-\", \"symbol\": \" \"},\n\t\t{\"phrase\": \"---..\", \"symbol\": \"8\"},\n\t\t{\"phrase\": \"----.\", \"symbol\": \"9\"},\n\t\t{\"phrase\": \"-----\", \"symbol\": \"0\"},\n\n\t\t{\"phrase\": \"..--..\", \"symbol\": \"?\"},\n\t\t{\"phrase\": \".-..-.\", \"symbol\": \"\\\"\"},\n\t\t{\"phrase\": \".-.-.-\", \"symbol\": \".\"},\n\t\t{\"phrase\": \".--.-.\", \"symbol\": \"@\"},\n\t\t{\"phrase\": \".----.\", \"symbol\": \"'\"},\n\t\t{\"phrase\": \"-....-\", \"symbol\": \"-\"},\n\t\t{\"phrase\": \"-.--.-\", \"symbol\": \")\"},\n\t\t{\"phrase\": \"--..--\", \"symbol\": \",\"},\n\t\t{\"phrase\": \"---...\", \"symbol\": \":\"}\n\t]\n}";
	const unsigned short dataLength = (sizeof(message) | 15) + 1; //will always be larger than message length to ensure null terminator applied, up to 16 bytes in the worst case (message length _is_ a multiple of 16)
	Serial.print("dataLength: ");
	Serial.println(dataLength);

	unsigned char rawData[dataLength];

	for(unsigned short i = 0; i < dataLength; i += 1) {
		if(i < sizeof(message) - 1) {
			rawData[i] = message[i];
		} else {
			rawData[i] = '\0';
		}
	}

	//Serial.print("raw:\t\t");
	//printArr(rawData, dataLength);

	const long startTime = micros();

	AES_init_ctx_iv(&ctx, key, initVector);
	AES_CBC_encrypt_buffer(&ctx, rawData, dataLength);
	//Serial.print("encrypted:\t");
	//printArr(rawData, dataLength, false);

	AES_init_ctx_iv(&ctx, key, initVector);
	AES_CBC_decrypt_buffer(&ctx, rawData, dataLength);
	//Serial.print("decrypted:\t");
	//printArr(rawData, dataLength);

	const long elapsed = micros() - startTime;

	Serial.print("Took ");
	Serial.print(elapsed);
	Serial.println(" micro seconds");

	//Takes ~1320 microseconds on Arduino Nano 33 IoT for 32 bytes
	//Takes ~83963 microseconds on Arduino Nano 33 IoT for 2208 bytes
}
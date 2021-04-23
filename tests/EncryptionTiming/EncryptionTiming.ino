#include <Arduino.h>
#include <HardwareSerial.h>

#include <stdint.h>

#include "src/tiny-AES-c-1.0.0/aes.hpp"


static const constexpr short MESSAGE_LENGTH = 32;


void loop() {}


void printArr(const uint8_t* const arr, bool cast = true) {
	for(unsigned short i = 0; i < MESSAGE_LENGTH; i += 1) {
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

	unsigned char message[] = "Hello, glEEmail";
	unsigned char rawData[MESSAGE_LENGTH];

	for(unsigned short i = 0; i < MESSAGE_LENGTH; i += 1) {
		if(i < sizeof(message) - 1) {
			rawData[i] = message[i];
		} else {
			rawData[i] = '\0';
		}
	}

	Serial.print("raw:\t\t");
	printArr(rawData);

	AES_init_ctx_iv(&ctx, key, initVector);
	AES_CBC_encrypt_buffer(&ctx, rawData, MESSAGE_LENGTH);
	Serial.print("encrypted:\t");
	printArr(rawData, false);

	AES_init_ctx_iv(&ctx, key, initVector);
	AES_CBC_decrypt_buffer(&ctx, rawData, MESSAGE_LENGTH);
	Serial.print("decrypted:\t");
	printArr(rawData);
}
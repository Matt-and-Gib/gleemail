#ifndef SECURITY_H
#define SECURITY_H

#define USE_TINY_C_AES 1

#if USE_TINY_C_AES
#include "lib/tiny-AES-c-1.0.0/aes.hpp"
#ifndef AES128
#error Must define AES128
#endif
#endif


class Security {
private:
#if USE_TINY_C_AES
	const static constexpr uint8_t key[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	const static constexpr uint8_t initializationVector[] = "iviviviviviviviv";

	static struct AES_ctx ctx;
#endif
public:
	static unsigned char* createPaddedStringFromBuffer(const char* buffer, const unsigned short length) {
		const unsigned short paddedLength = (length | 15) + 1;

		unsigned char paddedBuffer[paddedLength];
		for(unsigned short i = 0; i < paddedLength; i += 1) {
			if(i < length - 1) {
				paddedBuffer[i] = buffer[i];
			} else {
				paddedBuffer[i] = '\0';
			}
		}

		return paddedBuffer; //note: returning temporary address!
	}


	static void encryptString(char* buffer, const unsigned short length) {
#if USE_TINY_C_AES
		AES_init_ctx_iv(&ctx, key, initializationVector);
		AES_CBC_encrypt_buffer(&ctx, (unsigned char*)buffer, length);
#endif
	}


	static void decryptString(char* buffer, const unsigned short length) {
#if USE_TINY_C_AES
		AES_init_ctx_iv(&ctx, key, initializationVector);
		AES_CBC_decrypt_buffer(&ctx, (unsigned char*)buffer, length);
#endif
	}
};

#endif
#ifndef WEBACCESS_H
#define WEBACCESS_H

#include "internetaccess.h"


class WebAccess {
private:
	char* server;

	static const constexpr short DATA_BUFFER_SIZE = 3040; //Buffer Index rouned power of 2 //3035; Buffer index //3072; Suggested size

	static const constexpr char HEADER_END_STRING[] = "\r\n\r\n";
	static const constexpr unsigned short LENGTH_OF_HEADER_END_STRING = sizeof(HEADER_END_STRING)/sizeof(HEADER_END_STRING[0]) - 1;

	short findEndOfHeaderIndex(const char*, const unsigned short);
public:
	bool connectToServer(InternetAccess& net, const char* address) {return net.connectToWeb(address);}
	bool sendRequestToServer(InternetAccess&, const char* const*);

	char* downloadFromServer(InternetAccess&, const char*, const char* const*);
};


bool WebAccess::sendRequestToServer(InternetAccess& net, const char* const* headers) {
	const char* headerLine = headers[0];
	if(headerLine == nullptr) {
		return false;
	}

	int headerIndex = 0;
	while(headerLine != nullptr) {
		if(headerIndex > 16) { //Replace magic number
			DebugLog::getLog().logError(ERROR_CODE::NETWORK_HEADER_TERMINATION_OMITTED);
			return false;
		}

		net.writeHeaderLine(headerLine);
		headerLine = headers[++headerIndex];
	}

	return true;
}


short WebAccess::findEndOfHeaderIndex(const char* const rawData, const unsigned short lengthOfData) {
	unsigned short headerEndSearchIndex = 0;
	short endOfHeaderIndex = -1;

	for(unsigned short beginningSubstringIndex = 0; beginningSubstringIndex < lengthOfData; beginningSubstringIndex += 1) {
		if(rawData[beginningSubstringIndex] == '\r') {
			headerEndSearchIndex = 0;
			for(unsigned short endSubstringIndex = 0; endSubstringIndex < LENGTH_OF_HEADER_TERMINATION; endSubstringIndex += 1) {
				if(rawData[beginningSubstringIndex + endSubstringIndex] != HEADER_TERMINATION[headerEndSearchIndex++]) {
					goto headerEndMismatch;
				}
			}

			endOfHeaderIndex = beginningSubstringIndex + LENGTH_OF_HEADER_TERMINATION;
			break;
		}
		headerEndMismatch:;
	}

	return endOfHeaderIndex;
}


char* WebAccess::downloadFromServer(InternetAccess& net, const char* server, const char* const* headers) {
	if(WiFi.status() != WL_CONNECTED) {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_DOWNLOAD_IMPOSSIBLE_NOT_CONNECTED);
		return nullptr;
	}

	if(!net.connectToWeb(server)) {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_SECURE_CONNECTION_TO_SERVER_FAILED);
		return nullptr;
	}

	if(!sendRequestToServer(net, headers)) {
		DebugLog::getLog().logError(ERROR_CODE::NETWORK_REQUEST_TO_SERVER_HEADER_INVALID);
		return nullptr;
	}

	int bufferIndex = 0;
	char* dataBuffer = new char[DATA_BUFFER_SIZE];
	while(net.activeWebConnection()) {
		while(net.responseAvailableFromWeb()) {
			if(bufferIndex < DATA_BUFFER_SIZE) {
				dataBuffer[bufferIndex++] = net.nextCharInWebResponse();
			} else {
				DebugLog::getLog().logError(ERROR_CODE::NETWORK_DATA_BUFFER_OVERFLOW);
				return nullptr;
			}
		}
	}

	if(bufferIndex < DATA_BUFFER_SIZE/2) {
		DebugLog::getLog().logWarning(ERROR_CODE::NETWORK_DATA_BUFFER_UNDERUTILIZED);
	}

	/*//Print full response
	for(int i = 0; i < bufferIndex; i += 1) {
		Serial.print(dataBuffer[i]);
	}*/

	/*//Print buffer utilization
	Serial.print("Used ");
	Serial.print(bufferIndex);
	Serial.print(" out of max ");
	Serial.println(DATA_BUFFER_SIZE);*/

	short endOfHeaderIndex = findEndOfHeaderIndex(dataBuffer, bufferIndex);
	if(endOfHeaderIndex != -1) {
		const unsigned short LENGTH_OF_JSON_BODY = bufferIndex - endOfHeaderIndex;
		char* jsonData = new char[LENGTH_OF_JSON_BODY];
		for(int i = 0; i < LENGTH_OF_JSON_BODY; i += 1) {
			jsonData[i] = dataBuffer[endOfHeaderIndex + i];
		}
		jsonData[LENGTH_OF_JSON_BODY] = '\0';

		delete[] dataBuffer;
		return jsonData;
	}

	return nullptr;
}

#endif
#include "include/websiteaccess.h"

#include "include/global.h"

#include <WiFiNINA.h>

using namespace GLEEMAIL_DEBUG;


bool WebsiteAccess::writeHeadersToServer(InternetAccess& net, const char* const* headers) {
	const char* headerLine = headers[0];
	if(headerLine == nullptr) {
		return false;
	}

	int headerIndex = 0;
	while(headerLine != nullptr) {
		if(headerIndex > 16) { //Replace magic number
			DebugLog::getLog().logError(ERROR_CODE::WEB_ACCESS_HEADER_TERMINATION_OMITTED);
			return false;
		}

		net.writeHeaderLine(headerLine);
		headerLine = headers[++headerIndex];
	}

	return true;
}


bool WebsiteAccess::sendRequestToServer(InternetAccess& net, const char* server, const char* const* headers) {
	if(WiFi.status() != WL_CONNECTED) {
		DebugLog::getLog().logError(ERROR_CODE::WEB_ACCESS_DOWNLOAD_IMPOSSIBLE_NOT_CONNECTED);
		return false;
	}

	if(!net.connectToWeb(server)) {
		DebugLog::getLog().logError(ERROR_CODE::WEB_ACCESS_SECURE_CONNECTION_TO_SERVER_FAILED);
		return false;
	}

	if(!writeHeadersToServer(net, headers)) {
		DebugLog::getLog().logError(ERROR_CODE::WEB_ACCESS_REQUEST_TO_SERVER_HEADER_INVALID);
		return false;
	}

	return true;
}


short WebsiteAccess::findEndOfHeaderIndex(const char* const rawData, const unsigned short lengthOfData) {
	unsigned short headerEndSearchIndex = 0;
	short endOfHeaderIndex = -1;

	for(unsigned short beginningSubstringIndex = 0; beginningSubstringIndex < lengthOfData; beginningSubstringIndex += 1) {
		if(rawData[beginningSubstringIndex] == '\r') {
			headerEndSearchIndex = 0;
			for(unsigned short endSubstringIndex = 0; endSubstringIndex < LENGTH_OF_HEADER_TERMINATION; endSubstringIndex += 1) {
				if(rawData[beginningSubstringIndex + endSubstringIndex] != NETWORK_HEADER_TERMINATION[headerEndSearchIndex++]) {
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


char* WebsiteAccess::downloadFromServer(InternetAccess& net) {
	int bufferIndex = 0;
	char* dataBuffer = new char[DOWNLOADED_PACKET_BUFFER_SIZE];
	while(net.activeWebConnection()) {
		while(net.responseAvailableFromWeb()) {
			if(bufferIndex < DOWNLOADED_PACKET_BUFFER_SIZE) {
				dataBuffer[bufferIndex++] = net.nextCharInWebResponse();
			} else {
				DebugLog::getLog().logError(ERROR_CODE::WEB_ACCESS_DATA_BUFFER_OVERFLOW);
				delete[] dataBuffer;
				return nullptr;
			}
		}
	}

	dataBuffer[bufferIndex] = '\0';

	if(bufferIndex < DOWNLOADED_PACKET_BUFFER_SIZE/2) {
		DebugLog::getLog().logWarning(ERROR_CODE::WEB_ACCESS_DATA_BUFFER_UNDERUTILIZED);
	}

	//Print full response
	/*for(int i = 0; i < bufferIndex; i += 1) {
		Serial.print(dataBuffer[i]);
	}
	Serial.println('\n');*/

	/*//Print buffer utilization
	Serial.print("Used ");
	Serial.print(bufferIndex);
	Serial.print(" out of max ");
	Serial.println(DOWNLOADED_PACKET_BUFFER_SIZE);*/

	short endOfHeaderIndex = findEndOfHeaderIndex(dataBuffer, bufferIndex);
	if(endOfHeaderIndex != -1) {
		const unsigned short LENGTH_OF_JSON_BODY = bufferIndex - endOfHeaderIndex;

		//Serial.print(F("length of body: "));
		//Serial.println(LENGTH_OF_JSON_BODY);

		char* payloadData = new char[LENGTH_OF_JSON_BODY];
		for(int i = 0; i < LENGTH_OF_JSON_BODY; i += 1) {
			payloadData[i] = dataBuffer[endOfHeaderIndex + i];
		}
		payloadData[LENGTH_OF_JSON_BODY] = '\0';

		delete[] dataBuffer;
		return payloadData;
	}

	delete[] dataBuffer;
	return nullptr;
}
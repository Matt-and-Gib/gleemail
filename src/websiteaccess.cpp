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
		net.writeHeaderLine(headerLine);
		headerLine = headers[++headerIndex];
	}

	return true; //There currently is no check for if a terminator is somehow omitted from the headers that are passed in.
}


bool WebsiteAccess::sendRequestToServer(InternetAccess& net, const char* const server, const char* const* headers) {
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
				dataBuffer = nullptr;
				return nullptr;
			}
		}
	}

	dataBuffer[bufferIndex] = '\0';

	if(bufferIndex < DOWNLOADED_PACKET_BUFFER_SIZE/2) {
		if(bufferIndex > 0) {
			DebugLog::getLog().logWarning(ERROR_CODE::WEB_ACCESS_DATA_BUFFER_UNDERUTILIZED);
		} else {
			DebugLog::getLog().logError(ERROR_CODE::WEB_ACCESS_DATA_BUFFER_EMPTY);
			delete[] dataBuffer;
			dataBuffer = nullptr;
			return nullptr;
		}
	}

	short endOfHeaderIndex = findEndOfHeaderIndex(dataBuffer, bufferIndex);
	if(endOfHeaderIndex != -1) {
		const unsigned short LENGTH_OF_JSON_BODY = bufferIndex - endOfHeaderIndex;
		char* payloadData = new char[LENGTH_OF_JSON_BODY];
		for(int i = 0; i < LENGTH_OF_JSON_BODY; i += 1) {
			payloadData[i] = dataBuffer[endOfHeaderIndex + i];
		}
		payloadData[LENGTH_OF_JSON_BODY] = '\0';

		delete[] dataBuffer;
		return payloadData;
	} else {
		DebugLog::getLog().logError(ERROR_CODE::WEB_ACCESS_RESPONSE_HEADER_TERMINATION_NOT_FOUND);
		delete[] dataBuffer;
		dataBuffer = nullptr;
		return nullptr;
	}
}
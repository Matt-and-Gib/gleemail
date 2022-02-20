#ifndef WEBSITEACCESS_H
#define WEBSITEACCESS_H

#include "internetaccess.h"


class WebsiteAccess final {
private:
	char* server = nullptr;

	static const constexpr short DOWNLOADED_PACKET_BUFFER_SIZE = 3100; //Size of MorseCodeCharPairs Version 3 (2211) + calculated response header size (871) rounded up

	static const constexpr char HEADER_END_STRING[] = "\r\n\r\n";
	static const constexpr unsigned short LENGTH_OF_HEADER_END_STRING = sizeof(HEADER_END_STRING)/sizeof(HEADER_END_STRING[0]) - 1;

	bool writeHeadersToServer(InternetAccess&, const char* const*);
public:
	explicit WebsiteAccess() = default;
	~WebsiteAccess() = default;

	bool connectToServer(InternetAccess& net, const char* address) {return net.connectToWeb(address);}
	bool sendRequestToServer(InternetAccess& net, const char* server, const char* const* headers);
	char* downloadFromServer(InternetAccess&);

	static short findEndOfHeaderIndex(const char*, const unsigned short);
};

#endif
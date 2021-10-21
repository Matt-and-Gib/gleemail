#ifndef GLEEPAL_H
#define GLEEPAL_H

// To use in the future to map IP addresses (or uuids)
class glEEpal {
private:
	IPAddress palIPAddress;
	unsigned short outgoingHandshakeIdempotencyTokenValue;
public:
	glEEpal() {}

	glEEpal(const IPAddress ip, const unsigned short h) {
		palIPAddress = ip;
		outgoingHandshakeIdempotencyTokenValue = h;
	}

	bool operator==(glEEpal& o) {
		return palIPAddress == o.getIPAddress();
	}
	bool operator==(IPAddress a) {
		return palIPAddress == a;
	}

	const IPAddress getIPAddress() const {return palIPAddress;}
	const unsigned short getHandshakeIdempotencyTokenValue() const {return outgoingHandshakeIdempotencyTokenValue;}
};
glEEpal* glEEself = new glEEpal();

#endif
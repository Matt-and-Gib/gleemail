#ifndef GLEEPAL_H
#define GLEEPAL_H

//For use in the future to map IP addresses (or uuids)
class glEEpal final {
private:
	IPAddress palIPAddress;
	unsigned short outgoingHandshakeIdempotencyTokenValue;
public:
	glEEpal(const IPAddress ip, const unsigned short h) : palIPAddress{ip}, outgoingHandshakeIdempotencyTokenValue{h} {}
	glEEpal(const glEEpal&) = delete;
	glEEpal(glEEpal&&) = delete;
	glEEpal& operator=(const glEEpal&) = delete;
	glEEpal& operator=(glEEpal&&) = delete;
	~glEEpal() = default;

	bool operator==(const glEEpal& o) {
		return palIPAddress == o.getIPAddress();
	}
	
	bool operator==(const IPAddress& a) {
		return palIPAddress == a;
	}

	[[nodiscard]] const IPAddress getIPAddress() const {return palIPAddress;}
	[[nodiscard]] unsigned short getHandshakeIdempotencyTokenValue() const {return outgoingHandshakeIdempotencyTokenValue;}
};
glEEpal* glEEself = new glEEpal(IPAddress(), 0); //I don't think this is good design

#endif
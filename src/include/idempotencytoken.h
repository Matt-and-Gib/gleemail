#ifndef IDEMPOTENCYTOKEN_H
#define IDEMPOTENCYTOKEN_H

class IdempotencyToken final {
private:
	unsigned short value;
	unsigned long timestamp;
	unsigned short retryCount;
public:
	IdempotencyToken(const unsigned short v = 0, const unsigned long t = 0) : value{v}, timestamp{t}, retryCount{0} {}
	IdempotencyToken(const IdempotencyToken& i) : value{i.getValue()}, timestamp{i.getTimestamp()}, retryCount{i.getRetryCount()} {}
	
	~IdempotencyToken() = default;

	bool operator==(const IdempotencyToken& o) {return value == o.getValue();}

	unsigned short getValue() const {return value;}
	unsigned long getTimestamp() const {return timestamp;}
	unsigned short getRetryCount() const {return retryCount;}
	void incrementRetryCount() {retryCount += 1;}
};

#endif
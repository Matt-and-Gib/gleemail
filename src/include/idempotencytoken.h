#ifndef IDEMPOTENCYTOKEN_H
#define IDEMPOTENCYTOKEN_H

class IdempotencyToken {
private:
	unsigned short value;
	unsigned long timestamp;
	unsigned short retryCount;
public:
	IdempotencyToken() {
		value = 0;
		timestamp = 0;
		retryCount = 0;
	}
	IdempotencyToken(const unsigned short v, const unsigned long t) {
		value = v;
		timestamp = t;
		retryCount = 0;
	}
	IdempotencyToken(const IdempotencyToken& i) {
		value = i.getValue();
		timestamp = i.getTimestamp();
		retryCount = i.getRetryCount();
	}
	~IdempotencyToken() {

	}

	bool operator==(const IdempotencyToken& o) {return value == o.getValue();}

	unsigned short getValue() const {return value;}
	unsigned long getTimestamp() const {return timestamp;}
	unsigned short getRetryCount() const {return retryCount;}
	void incrementRetryCount() {retryCount += 1;}
};

#endif
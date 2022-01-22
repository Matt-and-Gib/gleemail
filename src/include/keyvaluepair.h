#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H

template <class K, class V>
class KVPair {
private:
	const K& key;
	V value;

public:
	KVPair(const K& k, V v) : key{k}, value{v} {}
	//~KVPair() {} //Implement this! This is important!
	const K& getKey() const {return key;}
	V getValue() {return value;}

	bool operator==(KVPair<K, V>& o) const {return key == o.getKey();}
	bool operator==(const K& o) const {return key == o;}
};

#endif
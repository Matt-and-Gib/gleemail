#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H

template <class K, class V>
class KVPair {
private:
	const K& key;
	const V& value;
public:
	KVPair(const K& k, const V& v) : key{k}, value{v} {}
	//~KVPair() {} //Implement this! This is important!
	const K& getKey() const {return key;}
	const V& getValue() const {return value;}

	bool operator==(KVPair<K, V>& o) const {return key == o.getKey();}
	bool operator==(const K& o) const {return key == o;}
};

#endif
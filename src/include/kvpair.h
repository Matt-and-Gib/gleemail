#ifndef KVPAIR_H
#define KVPAIR_H

template <class K, class V>
class KVPair {
private:
	const K& key;
	const V& value;
public:
	KVPair() {}
	KVPair(const K& k, const V& v) : key{k}, value{v} {}
	const K& getKey() const {return key;}

	bool operator==(KVPair<K, V>& o) const {return key == o.getKey();}
};

#endif
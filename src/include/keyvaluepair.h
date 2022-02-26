#ifndef KEY_VALUE_PAIR_H
#define KEY_VALUE_PAIR_H

template <class K, class V>
class KVPair final {
private:
	K key;
	V value;

public:
	KVPair(K k, V v) : key{k}, value{v} {}
	KVPair(const KVPair&) = delete;
	KVPair(KVPair&&) = delete;
	KVPair& operator=(const KVPair&) = delete;
	KVPair& operator=(KVPair&&) = delete;
	~KVPair() = default;
	
	K getKey() {return key;}
	V getValue() {return value;}

	bool operator==(KVPair<K, V>& o) const {return key == o.getKey();}
	bool operator==(const K& o) const {return key == o;}
};

#endif
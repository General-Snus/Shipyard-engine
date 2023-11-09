#pragma once
#include <vector>
#include <string>
 
	enum State : uint16_t
	{
		Empty = 1 << 0,
		InUse = 1 << 1,
		Removed = 1 << 2,
	};
	template <class Key, class Value>
	class HashMap
	{
	public:
		struct Entry
		{
			Entry() = default;
			Entry(State aState, Key aKey, Value aValue)
			{
				state = aState;
				key = aKey;
				value = aValue;
			};

			State state;
			Key key;
			Value value;
		};


		//Skapar en HashMap med plats för aCapacity element. Den behöver inte
		//kunna växa. Blir den full så blir den full.
		HashMap(int aCapacity);
		HashMap();
		~HashMap();

		//Stoppar in aValue under nyckeln aKey. Om det fanns ett värde innan med
		//aKey som nyckel så skrivs det värdet över. Om det lyckas returneras
		//true. Om map:en är full så görs ingenting, och false returneras.
		bool Insert(const Key& aKey, const Value& aValue);
		//Plockar bort elementet med nyckeln aKey, om det finns. Om det finns
		//returneras true. Om det inte finns görs ingenting, och false
		//returneras.
		bool Remove(const Key& aKey);
		//Ger en pekare till värdet med nyckeln aKey, om det finns. Om det inte
		//finns returneras nullptr.
		const Value* Get(const Key& aKey) const;
		//Som ovan, men returnerar en icke-const-pekare.
		Value* Get(const Key& aKey);

	private:
		bool IsEqual(const Key& lhs, const Key& rhs) const;

		std::vector<Entry*> myEntries;
		int tableSize;
		int lockedElements;
	};
	template <class Key>
	uint32_t Hash(const Key& aKey);
	uint32_t Hash(const std::string& aKey);
	uint32_t MurmurHash2A(const void* key, int len, uint32_t seed);

	template <class Key, class Value>
	bool HashMap<Key, Value>::IsEqual(const Key& lhs, const Key& rhs) const
	{
		if ((!(lhs < rhs) && !(rhs < lhs)))
		{
			return true;
		}
		return false;
	}

	template<class Key, class Value>
	inline HashMap<Key, Value>::HashMap(int aCapacity)
	{
		tableSize = aCapacity;
		myEntries.resize(aCapacity);
		lockedElements = 0;

		for (auto& var : myEntries)
		{
			Entry* insertEmpty = new Entry();
			insertEmpty->state = State::Empty;
			insertEmpty->value = Value();
			var = insertEmpty;
		}
	}
	template<class Key, class Value>
	inline HashMap<Key, Value>::HashMap() : HashMap(500)
	{
	}
	template<class Key, class Value>
	inline HashMap<Key, Value>::~HashMap()
	{
		for (auto& var : myEntries)
		{
			delete var;
		}
		myEntries.clear();
	}
	template<class Key, class Value>
	inline bool HashMap<Key, Value>::Insert(const Key& aKey, const Value& aValue)
	{
		if (tableSize < 1)
		{
			return false;
		}

		int count = 0;
		int index = Hash(aKey) % tableSize; 

		while (myEntries[index]->state & State::InUse && !IsEqual(myEntries[index]->key, aKey))
		{
			index = (index + 1) % tableSize;
			count++;

			if (count > tableSize)
			{
				return false;
			}
		}

		if (myEntries[index]->state != State::InUse)
		{
			lockedElements++;
		}

		myEntries[index]->key = aKey;
		myEntries[index]->value = aValue;
		myEntries[index]->state = State::InUse;
		return true;
	}
	template<class Key, class Value>
	inline bool HashMap<Key, Value>::Remove(const Key& aKey)
	{
		Entry* temporaryEntry = nullptr;
		int counter = 0;
		int index = Hash(aKey) % tableSize;

		if (lockedElements == 0)
		{
			return false;
		}

		while (!IsEqual(myEntries[index]->key, aKey) && counter < tableSize)
		{
			index = (index + 1) % tableSize;
			counter++;
		}

		if (counter >= tableSize)
		{
			return false;
		}

		temporaryEntry = myEntries[index];
		temporaryEntry->state = State::Removed;
		lockedElements--;
		return true;
	}
	template<class Key, class Value>
	inline const Value* HashMap<Key, Value>::Get(const Key& aKey) const
	{
		return new Value(this->Get(aKey));
	}
	template<class Key, class Value>
	inline Value* HashMap<Key, Value>::Get(const Key& aKey)
	{
		int counter = 0;
		int index = Hash(aKey) % tableSize;

		while (counter < tableSize)
		{
			if (myEntries[index]->state & State::InUse)
			{
				if (IsEqual(myEntries[index]->key, aKey))
				{
					break;
				}
			}
			index = (index + 1) % tableSize;
			counter++;
		}

		if (counter >= tableSize)
		{
			return nullptr;
		}

		return &myEntries[index]->value;

	}


	inline uint32_t MurmurHash2A(const void* key, int len, uint32_t seed)
	{
#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }


		const uint32_t m = 0x5bd1e995;
		const int r = 24;
		uint32_t l = len;

		const unsigned char* data = (const unsigned char*)key;

		uint32_t h = seed;

		while (len >= 4)
		{
			uint32_t k = *(uint32_t*)data;

			mmix(h, k);

			data += 4;
			len -= 4;
		}

		uint32_t t = 0;

		switch (len)
		{
		case 3: t ^= data[2] << 16;
		case 2: t ^= data[1] << 8;
		case 1: t ^= data[0];
		};

		mmix(h, t);
		mmix(h, l);

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;

		return h;
	}

	template<class Key>
	inline uint32_t Hash(const Key& aKey)
	{
		return MurmurHash2A(&aKey, sizeof(Key), 3984759834);
	}

	inline uint32_t Hash(const std::string& aKey)
	{
		return MurmurHash2A(aKey.c_str(), (int)aKey.size(), 3984759834);
	} 
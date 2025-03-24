#pragma once
#include "../TemplateHelpers.h"
#include <array>

template<typename T,size_t size>
class CircularBuffer
{
public:
	T& Add(const T& add) {
		data[index] = add;
		index = (index + 1) % size;
		if(currentSize < size) {
			++currentSize;
		}

		return data[index];
	};
	T& Get(int at)
	{
		return data[at % size];
	};

	T& operator [](int idx) {
		return data[idx];
	}

	// Iterator support
	auto begin() {
		return data.begin();
	}

	auto end() {
		return data.begin() + currentSize;
	}

	auto begin() const {
		return data.begin();
	}

	auto end() const {
		return data.begin() + currentSize;
	}

	//Sum of all added values
	T Sum() const;

	constexpr size_t Count() const {
		return size;
	}
private:
	int index = 0;
	int currentSize = 0;
	std::array<T,size> data;
};

template<typename T, size_t size>
T CircularBuffer<T, size>::Sum() const
{
	static_assert(Arithmetic<T>);
    T sum{};
    for (int i = 0; i < currentSize; ++i)
    {
        sum += data[i];
    }
    return sum;
}
#pragma once
template <typename T>
class CustomRange
{
	T* begin_ptr;
	T* end_ptr;

public:
	class Iterator
	{
		T* current;

	public:
		Iterator(T* ptr) : current(ptr)
		{
		}

		T& operator*() { return *current; }

		Iterator& operator++()
		{
			++current;
			return *this;
		}

		bool operator!=(const Iterator& other) const
		{
			return current != other.current;
		}
	};

	CustomRange(T* start, T* end) : begin_ptr(start), end_ptr(end)
	{
	}

	Iterator begin() { return Iterator(begin_ptr); }
	Iterator end() { return Iterator(end_ptr); }
};

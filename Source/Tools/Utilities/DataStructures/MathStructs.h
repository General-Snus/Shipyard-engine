#pragma once


template<typename T, size_t max_size>
class Avg {
public:
	T Average() const
	{
		return cursor == 0 ? sum : sum / cursor;
	}

	void Add(T add)
	{
		if (cursor > max_size)
		{
			cursor = cursor / 2;
			sum = sum / 2;
		}

		sum += add;
		cursor++;
	}

private:
	int cursor = 0;
	T sum{};
};

#pragma once
#include <math.h>
#include <Tools/Utilities/Math.hpp>

template <typename T = float>
class Range
{
public:
	Range(T min, T max) : from(min), to(max)
	{
		static_assert(std::is_arithmetic_v<T>, "Range only supports arithmetic types");
		assert(min < max);
	}

	T random(const bool inclusive = false) const
	{
		if (!inclusive)
		{
			return Math::RandomEngine::randomInRange<T>(from, to);
		}
		return Math::RandomEngine::randomInRange<T>(from, to + 1);
	}

	T lerp(float t) const
	{
		return from + t * (to - from);
	}

	T length()
	{
		return to - from;
	}

	bool contains(T value)
	{
		return value >= from && value <= to;
	}

	bool Intersect(Range<T> value)
	{
		return std::max(from, value.from) <= std::min(to, value.to);
	}

	T from;
	T to;
};

using FloatRange = Range<float>;
using IntRange = Range<int>;

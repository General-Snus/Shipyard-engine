#pragma once
#include <Tools/Utilities/Math.hpp>
#include <math.h>

template <typename T = float> class Range
{
  public:
    Range(T min, T max) : from(min), to(max)
    {
        static_assert(std::is_arithmetic<T>::value, "Range only supports arithmetic types");
        assert(min < max);
    }

    T random(const bool inclusive = false) const
    {
        if (!inclusive)
        {
            return RandomEngine::RandomInRange<T>(from, to);
        }
        else
        {
            return RandomEngine::RandomInRange<T>(from, to + 1);
        }
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

#pragma once
#include <algorithm>
#include <ranges>

template <typename T>
T lerp(T aStartValue, T aEndValue, float t)
{
	const float val = std::clamp<float>(t, 0, 1); // assures that the given parameter "t" is between 0 and 1

	if constexpr (std::ranges::range<T>)
	{
		T out{};
		for (auto [result, start, end] : std::views::zip(out, aStartValue, aEndValue))
		{
			result = start + (end - start) * val;
		}
		return out;
	}
	else
	{
		return aStartValue + (aEndValue - aStartValue) * val;
	}
}

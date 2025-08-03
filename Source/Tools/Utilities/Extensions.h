#pragma once 
#include <functional>
#include "TemplateHelpers.h"

namespace Extensions {
	//BoostHash
	template<Hashable T>
	void hash_combine(std::size_t& seed, T value)
	{
		seed ^= std::hash<T>{}(value)+0x9e3779b9 + (seed << 6) + (seed >> 2);
	};
}

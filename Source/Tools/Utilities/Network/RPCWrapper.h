#pragma once
#include <array>

class RPC
{
public:
	auto reg()
	{ 
		return 126 + 1;
	}
};


template<typename F>
constexpr void a(F&& func)
{
	using a = func;
}
void CallAsRPC(auto&& foo)
{
	foo();
};

#pragma once
#include <array>

template <typename Type, int size, typename CountType = unsigned short, bool UseSafeModeFlag = true>
class VectorOnStack
{
public:
	VectorOnStack();

	VectorOnStack(const VectorOnStack& aVectorOnStack)
	{
	}

	~VectorOnStack()
	{
	}

	VectorOnStack& operator=(const VectorOnStack&
		aVectorOnStack)
	{
	}

	const Type& operator[](const CountType aIndex) const
	{
	}


	void Add(const Type& aObject)
	{
	}

	void Insert(const CountType aIndex, const Type& aObject)
	{
	}

	void RemoveCyclic(const Type& aObject)
	{
	}

	void RemoveCyclicAtIndex(const CountType aIndex)
	{
	}

	void Clear()
	{
	}

	__forceinline CountType Size() const
	{
	}

private:
	int                    count;
	std::array<Type, size> array;
};

template <typename Type, int size, typename CountType, bool UseSafeModeFlag>
VectorOnStack<Type, size, CountType, UseSafeModeFlag>::VectorOnStack()
{
}

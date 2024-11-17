#pragma once 
#include <array> 

enum VectorIndices
{
	zero = 0,
	x = 1,
	y = 2,
	z = 3,
	w = 4,

	Zero = 0,
	X = 1,
	Y = 2,
	Z = 3,
	W = 4
};

template <typename T, unsigned N> class VectorX
{
  public:
	VectorX() = default;
	template <typename...Ty> constexpr VectorX(Ty... values) : data(values...)
	{
		static_assert((sizeof...(values)) == N, "NON EQUAL LENGTH WITH DECLARED TYPE");
	}

	T &operator[](int index)
	{
		return data[index];
	}
	const T &operator[](int index) const
	{
		return data[index];
	}

	template <int... Indices> VectorX<T, sizeof...(Indices)> swizzle() const
	{
		constexpr size_t swizzleSize = sizeof...(Indices);
		return VectorX<T, swizzleSize>(getIndex<Indices>()...);
	}

	//Vector2<T> V2() const
	//{
	//	static_assert(N == 2, "VectorX must have 2 elements to convert to Vector2");
	//	return Vector2<T>(getIndex<1>(), getIndex<2>());
	//}
	//
	//Vector3<T> V3() const
	//{
	//	static_assert(N == 3, "VectorX must have 3 elements to convert to Vector3");
	//	return Vector3<T>(getIndex<1>(), getIndex<2>(), getIndex<3>());
	//}
	//
	//Vector4<T> V4() const
	//{
	//	static_assert(N == 4, "VectorX must have 4 elements to convert to Vector4");
	//	return Vector4<T>(getIndex<1>(), getIndex<2>(), getIndex<3>(), getIndex<4>());
	//}

  private:
	std::array<T, N> data;

	template <int index> constexpr T getIndex() const
	{
		static_assert(int(index - 1) < int(N), "Index out of range for VectorX");
		static_assert(int(index + 1) > -int(N), "Index out of range for VectorX");
		if constexpr (index == 0)
			return 0;
		else if constexpr (index < 0)
			return -data[(index * -1) - 1];
		else
			return data[index - 1];
	}
};
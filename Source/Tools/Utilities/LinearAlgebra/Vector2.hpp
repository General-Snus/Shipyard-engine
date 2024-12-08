#pragma once
#include <assert.h>
#include <compare>
#include "VectorX.hpp"

template <class T>
class Vector2
{
public:
	T x;
	T y;
	// Creates a null-vector
	Vector2<T>();
	// Creates a vector (aX, aY)
	Vector2<T>(const T& aX, const T& aY);
	Vector2<T>(const Vector2<T>& aVector) = default;
	Vector2<T>(const VectorX<T, 2>& aVector);

	// Assignment operator (compiler generated)
	Vector2<T>& operator=(const Vector2<T>& aVector2) = default;
	Vector2<T>& operator=(const T& aTypeT);
	// Destructor (compiler generated)
	~Vector2<T>() = default;


	static constexpr std::size_t size() { return {2}; };

	T* begin() { return &x; };
	T* end() { return &x + size(); };
	T* begin() const { return &x; };
	T* end() const { return &x + size(); };


	// Implicit cast operator to any datatype, return initializer list of x, y, z
	template <class U>
	explicit operator U() const;

	operator std::initializer_list<T>() const
	{
		return {x, y};
	}

	operator std::array<T, 2>() const
	{
		return {x, y};
	}

	// Returns the negated vector
	Vector2<T> operator-() const;

	// Returns the array vector
	T&       operator[](int value);
	const T& operator[](int value) const;

	// Returns the squared length of the vector
	T LengthSqr() const;

	// Returns the length of the vector
	T Length() const;

	// Returns a normalized copy of this
	Vector2<T> GetNormalized() const;

	// Normalizes the vector
	void Normalize();

	// Returns the dot product of this and aVector
	T Dot(const Vector2<T>& aVector) const;

	Vector2<T> xx();
	Vector2<T> xy();
	Vector2<T> yx();
	Vector2<T> yy();
};

template <class T>
std::strong_ordering operator<=>(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	if (auto cmp = aVector0.x <=> aVector1.x; cmp != 0)
	{
		return cmp;
	}
	return aVector0.y <=> aVector1.y;
}

template <class T>
bool operator!=(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	return (aVector0 <=> aVector1) != 0;
}

// Returns the vector sum of aVector0 and aVector1
template <class T>
Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	return Vector2<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
}

// Returns the vector difference of aVector0 and aVector1
template <class T>
Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	return Vector2<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y);
}

// Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
{
	return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
}

// Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
{
	return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
}

// Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
template <class T>
Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
{
	const T mlt = (1 / aScalar);

	return Vector2<T>(aVector.x * mlt, aVector.y * mlt);
}

// Equivalent to setting aVector0 to (aVector0 + aVector1)
template <class T>
void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	aVector0.x = aVector0.x + aVector1.x;
	aVector0.y = aVector0.y + aVector1.y;
}

// Equivalent to setting aVector0 to (aVector0 - aVector1)
template <class T>
void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	aVector0.x = aVector0.x - aVector1.x;
	aVector0.y = aVector0.y - aVector1.y;
}

// Equivalent to setting aVector to (aVector * aScalar)
template <class T>
void operator*=(Vector2<T>& aVector, const T& aScalar)
{
	aVector.x = aVector.x * aScalar;
	aVector.y = aVector.y * aScalar;
}

// Equivalent to setting aVector to (aVector / aScalar)
template <class T>
void operator/=(Vector2<T>& aVector, const T& aScalar)
{
	aVector.x = aVector.x / aScalar;
	aVector.y = aVector.y / aScalar;
}

template <class T>
Vector2<T>& Vector2<T>::operator=(const T& aTypeT)
{
	x, y = aTypeT;
	return *this;
}

template <class T>
Vector2<T> Vector2<T>::operator-() const
{
	return Vector2<T>(-x, -y);
}

template <class T>
T Vector2<T>::LengthSqr() const
{
	return static_cast<T>(x * x + y * y);
}

template <class T>
T Vector2<T>::Length() const
{
	return static_cast<T>(sqrt(LengthSqr()));
}

template <class T>
Vector2<T> Vector2<T>::GetNormalized() const
{
	const T len = 1 / Length();
	return Vector2<T>(x * len, y * len);
}

template <class T>
void Vector2<T>::Normalize()
{
	const T len = 1 / Length();

	x = x * len;
	y = y * len;
}

template <class T>
T Vector2<T>::Dot(const Vector2<T>& aVector) const
{
	return x * aVector.x + y * aVector.y;
}

template <class T>
Vector2<T>::Vector2()
{
	x = 0;
	y = 0;
}

template <class T>
Vector2<T>::Vector2(const T& aX, const T& aY) : x(aX), y(aY)
{
}

template <class T>
Vector2<T>::Vector2(const VectorX<T, 2>& aVector) : x(aVector[0]), y(aVector[1])
{
}


#pragma warning(push)
#pragma warning(disable : 4244)
template <class T>
template <class U>
Vector2<T>::operator U() const
{
	return U(x, y);
}
#pragma warning(pop)

template <class T>
T& Vector2<T>::operator[](int value)
{
	assert(value >= 0 && value <= 2);
	switch (value)
	{
	case 0:
		{
			return x;
		}
	case 1:
		{
			return y;
		}
	default:
		{
			return x;
		}
	}
}

template <class T>
const T& Vector2<T>::operator[](int value) const
{
	assert(value >= 0 && value <= 2);
	switch (value)
	{
	case 0:
		{
			return x;
		}
	case 1:
		{
			return y;
		}
	default:
		{
			return x;
		}
	}
}

using Vector2f = Vector2<float>;
using Vector2i = Vector2<int>;
using Vector2ui = Vector2<unsigned int>;

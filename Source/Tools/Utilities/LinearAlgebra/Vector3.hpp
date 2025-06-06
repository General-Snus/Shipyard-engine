#pragma once
#include <assert.h>
#include <cmath>
#include <ostream>
#include "VectorX.hpp"

template <class T>
class Vector3
{
public:
	T x;
	T y;
	T z;
	// Creates a null-vector
	constexpr Vector3<T>();
	// Creates a vector (aX, aY, aZ)
	constexpr Vector3<T>(const T& aX, const T& aY, const T& aZ);
	constexpr Vector3<T>(const T& aValue);
	Vector3<T>(const VectorX<T, 3>& aVector);

	bool IsNearlyEqual(const Vector3<T>& cmp,float epsilon = 1e-5) const;

	// Copy constructor (compiler generated)
	constexpr Vector3<T>(const Vector3<T>& aVector) = default;
	// Assignment operator (compiler generated)
	constexpr Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
	constexpr Vector3<T>& operator=(const T& aTypeT);

	// Destructor (compiler generated)
	~Vector3<T>() = default;

	static constexpr std::size_t size() { return {3}; };

	T* begin() { return &x; };
	T* end() { return &x + size(); };
	T* begin() const { return &x; };
	T* end() const { return &x + size(); };

	// Implicit cast operator to any datatype, return initializer list of x, y, z
	template <class U>
	constexpr operator U() const;

	// Returns the negated vector
	constexpr Vector3<T> operator-() const;

	constexpr T&       operator[](int value);
	constexpr const T& operator[](int value) const;

	// Returns the squared length of the vector
	constexpr T LengthSqr() const;

	// Returns the length of the vector
	constexpr T Length() const;

	// Returns a normalized copy of this
	constexpr Vector3<T> GetNormalized() const;

	// Normalizes the vector
	constexpr void Normalize();

	// Returns the dot product of this and aVector
	constexpr T Dot(const Vector3<T>& aVector) const;

	// Returns the cross product of this and aVector
	constexpr Vector3<T> Cross(const Vector3<T>& aVector) const;

	static constexpr Vector3<T> right();
	static constexpr Vector3<T> up();
	static constexpr Vector3<T> forward();

	// Swizzles start at 1
	template <int... Indices>
	VectorX<T, sizeof...(Indices)> swizzle() const
	{
		// static_assert(((Indices > Z) && ...), "Invalid Value");
		// static_assert(((Indices < -Z) && ...), "Invalid Value");
		return VectorX<T, sizeof...(Indices)>(getIndex<Indices>()...);
	}
	std::string	toString();
private:
	template <int index>
	constexpr T getIndex() const
	{
		if constexpr (index == 0)
		{
			return 0; // Zero
		}
		else if constexpr (index == 1)
		{
			return x;
		}
		else if constexpr (index == 2)
		{
			return y;
		}
		else if constexpr (index == 3)
		{
			return z;
		}
		else if constexpr (index == -1)
		{
			return -x;
		}
		else if constexpr (index == -2)
		{
			return -y;
		}
		else if constexpr (index == -3)
		{
			return -z;
		}
		else
		{
			static_assert(index < 3, "Index out of range for Vector3");
		}
	}
};

#pragma region Operators
template <class T>
Vector3<T> MinVector(const Vector3<T>& a, const Vector3<T>& b)
{
	return Vector3<T>((a.x < b.x) ? a.x : b.x, (a.y < b.y) ? a.y : b.y, (a.z < b.z) ? a.z : b.z);
}

template <class T>
Vector3<T> MaxVector(const Vector3<T>& a, const Vector3<T>& b)
{
	return Vector3<T>((a.x > b.x) ? a.x : b.x, (a.y > b.y) ? a.y : b.y, (a.z > b.z) ? a.z : b.z);
}

// Returns the vector sum of aVector0 and aVector1
template <class T>
Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
}

// Returns the vector difference of aVector0 and aVector1
template <class T>
Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
}

// Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
{
	return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
}

// Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector3<T> operator*(const Vector3<T>& aVector, const Vector3<T>& aVector2)
{
	return Vector3<T>(aVector.x * aVector2.x, aVector.y * aVector2.y, aVector.z * aVector2.z);
}

// Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
{
	return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
}

// Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
template <class T>
Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
{
	const T mlt = (1 / aScalar);

	return Vector3<T>(aVector.x * mlt, aVector.y * mlt, aVector.z * mlt);
}

// Equivalent to setting aVector0 to (aVector0 + aVector1)
template <class T>
void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	aVector0.x = aVector0.x + aVector1.x;
	aVector0.y = aVector0.y + aVector1.y;
	aVector0.z = aVector0.z + aVector1.z;
}

// Equivalent to setting aVector0 to (aVector0 - aVector1)
template <class T>
void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	aVector0.x = aVector0.x - aVector1.x;
	aVector0.y = aVector0.y - aVector1.y;
	aVector0.z = aVector0.z - aVector1.z;
}

// Equivalent to setting aVector to (aVector * aScalar)
template <class T>
void operator*=(Vector3<T>& aVector, const T& aScalar)
{
	aVector.x = aVector.x * aScalar;
	aVector.y = aVector.y * aScalar;
	aVector.z = aVector.z * aScalar;
}

// Equivalent to setting aVector to (aVector / aScalar)
template <class T>
void operator/=(Vector3<T>& aVector, const T& aScalar)
{
	aVector.x = aVector.x / aScalar;
	aVector.y = aVector.y / aScalar;
	aVector.z = aVector.z / aScalar;
}

template <class T>
constexpr Vector3<T>& Vector3<T>::operator=(const T& aTypeT)
{
	x, y, z = aTypeT;
	return *this;
}

#pragma endregion

template <class T>
constexpr Vector3<T> Vector3<T>::operator-() const
{
	return Vector3<T>(-x, -y, -z);
}

template <class T>
constexpr T Vector3<T>::LengthSqr() const
{
	return static_cast<T>(x * x + y * y + z * z);
}

template <class T>
constexpr T Vector3<T>::Length() const
{
	return static_cast<T>(std::sqrt(LengthSqr()));
}

template <class T>
constexpr Vector3<T> Vector3<T>::GetNormalized() const
{
	const T len = 1 / Length();

	if (len != len || std::isinf(len)) // IEEE FLOAT NAN
	{
		return Vector3<T>(0, 0, 0);
	}

	return Vector3<T>(x * len, y * len, z * len);
}

template <class T>
constexpr void Vector3<T>::Normalize()
{
	const T len = 1 / Length();

	if (len != len || std::isinf(len)) // IEEE FLOAT NAN
	{
		x = 0;
		y = 0;
		z = 0;
		return;
	}

	x = x * len;
	y = y * len;
	z = z * len;
}

template <class T>
constexpr T Vector3<T>::Dot(const Vector3<T>& aVector) const
{
	return x * aVector.x + y * aVector.y + z * aVector.z;
}

template <class T>
constexpr Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
{
	return Vector3<T>(y * aVector.z - aVector.y * z, z * aVector.x - aVector.z * x, x * aVector.y - aVector.x * y);
}

template <class T>
constexpr Vector3<T> Vector3<T>::right()
{
	return Vector3<T>(1, 0, 0);
}

template <class T>
constexpr Vector3<T> Vector3<T>::up()
{
	return Vector3<T>(0, 1, 0);
}

template <class T>
constexpr Vector3<T> Vector3<T>::forward()
{
	return Vector3<T>(0, 0, 1);
}

template<class T>
inline std::string Vector3<T>::toString() {
	return std::format("{:3} {:3} {:3}",x,y,z);
}

template <class T>
constexpr Vector3<T>::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

template <class T>
constexpr Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ)
{
	x = aX;
	y = aY;
	z = aZ;
}

template <class T>
constexpr Vector3<T>::Vector3(const T& aValue)
{
	x = aValue;
	y = aValue;
	z = aValue;
}

template <class T>
Vector3<T>::Vector3(const VectorX<T, 3>& aVector) : x(aVector[0]), y(aVector[1]), z(aVector[2])
{
}

template <class T>
bool Vector3<T>::IsNearlyEqual(const Vector3<T>& cmp,float epsilon) const	{
	 
	 return 
		 std::abs(x - cmp.x) <= epsilon &&
		 std::abs(y - cmp.y) <= epsilon &&
		 std::abs(z - cmp.z) <= epsilon;
		 
}

template <class T>
template <class U>
constexpr Vector3<T>::operator U() const
{
	return {x, y, z};
}

template <class T>
constexpr T& Vector3<T>::operator[](int value)
{
	assert(value >= 0 && value <= 3);
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
	case 2:
		{
			return z;
		}
	default:
		{
			return x;
		}
	}
}

template <class T>
std::ostream& operator<<(std::ostream& anOstream, const Vector3<T>& aVector)
{
	anOstream << "{x: " << aVector.x << ", y: " << aVector.y << ", z: " << aVector.z << "}";
	return anOstream;
}

template <class T>
constexpr const T& Vector3<T>::operator[](int value) const
{
	assert(value >= 0 && value <= 3);
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
	case 2:
		{
			return z;
		}
	default:
		{
			return x;
		}
	}
}

using Vector3f = Vector3<float>;
using Vector3i = Vector3<int>;
using Vector3ui = Vector3<unsigned int>;

#pragma once
#include <assert.h> 
#include <cmath>
#include <ostream>

template <class T>
class Vector3
{
public:
	T x;
	T y;
	T z;
	//Creates a null-vector
	Vector3<T>();
	//Creates a vector (aX, aY, aZ)
	Vector3<T>(const T& aX,const T& aY,const T& aZ);
	Vector3<T>(const T& aValue);

	//Copy constructor (compiler generated)
	Vector3<T>(const Vector3<T>& aVector) = default;
	//Assignment operator (compiler generated)
	Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
	Vector3<T>& operator=(const T& aTypeT);
	//Destructor (compiler generated)
	~Vector3<T>() = default;

	//Implicit cast operator to any datatype, return initializer list of x, y, z
	template <class U> operator U() const;

	//Returns the negated vector
	Vector3<T> operator-() const;

	T& operator[](int value);
	const T& operator[](int value) const;

	//Returns the squared length of the vector
	T LengthSqr() const;

	//Returns the length of the vector
	T Length() const;

	//Returns a normalized copy of this
	Vector3<T> GetNormalized() const;

	//Normalizes the vector
	void Normalize();

	//Returns the dot product of this and aVector
	T Dot(const Vector3<T>& aVector) const;

	//Returns the cross product of this and aVector
	Vector3<T> Cross(const Vector3<T>& aVector) const;
};

#pragma region Operators
template <class T> inline Vector3<T> MinVector(const Vector3<T>& a,const Vector3<T>& b)
{
	return Vector3<T>(
		(a.x < b.x) ? a.x : b.x,
		(a.y < b.y) ? a.y : b.y,
		(a.z < b.z) ? a.z : b.z
	);
}

template <class T> inline Vector3<T> MaxVector(const Vector3<T>& a,const Vector3<T>& b)
{
	return Vector3<T>(
		(a.x > b.x) ? a.x : b.x,
		(a.y > b.y) ? a.y : b.y,
		(a.z > b.z) ? a.z : b.z
	);
}

//Returns the vector sum of aVector0 and aVector1
template <class T>
Vector3<T> operator+(const Vector3<T>& aVector0,const Vector3<T>& aVector1)
{
	return Vector3<T>(
		aVector0.x + aVector1.x,
		aVector0.y + aVector1.y,
		aVector0.z + aVector1.z
	);
}

//Returns the vector difference of aVector0 and aVector1
template <class T>
Vector3<T> operator-(const Vector3<T>& aVector0,const Vector3<T>& aVector1)
{
	return Vector3<T>(
		aVector0.x - aVector1.x,
		aVector0.y - aVector1.y,
		aVector0.z - aVector1.z
	);
}

//Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector3<T> operator*(const Vector3<T>& aVector,const T& aScalar)
{
	return Vector3<T>(
		aVector.x * aScalar,
		aVector.y * aScalar,
		aVector.z * aScalar
	);
}

//Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector3<T> operator*(const Vector3<T>& aVector,const Vector3<T>& aVector2)
{
	return Vector3<T>(
		aVector.x * aVector2.x,
		aVector.y * aVector2.y,
		aVector.z * aVector2.z
	);
}

//Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector3<T> operator*(const T& aScalar,const Vector3<T>& aVector)
{
	return Vector3<T>(
		aVector.x * aScalar,
		aVector.y * aScalar,
		aVector.z * aScalar
	);
}

//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
template <class T>
Vector3<T> operator/(const Vector3<T>& aVector,const T& aScalar)
{
	const T mlt = (1 / aScalar);

	return Vector3<T>(
		aVector.x * mlt,
		aVector.y * mlt,
		aVector.z * mlt
	);
}

//Equivalent to setting aVector0 to (aVector0 + aVector1)
template <class T>
void operator+=(Vector3<T>& aVector0,const Vector3<T>& aVector1)
{
	aVector0.x = aVector0.x + aVector1.x;
	aVector0.y = aVector0.y + aVector1.y;
	aVector0.z = aVector0.z + aVector1.z;
}

//Equivalent to setting aVector0 to (aVector0 - aVector1)
template <class T>
void operator-=(Vector3<T>& aVector0,const Vector3<T>& aVector1)
{
	aVector0.x = aVector0.x - aVector1.x;
	aVector0.y = aVector0.y - aVector1.y;
	aVector0.z = aVector0.z - aVector1.z;
}

//Equivalent to setting aVector to (aVector * aScalar)
template <class T>
void operator*=(Vector3<T>& aVector,const T& aScalar)
{
	aVector.x = aVector.x * aScalar;
	aVector.y = aVector.y * aScalar;
	aVector.z = aVector.z * aScalar;
}

//Equivalent to setting aVector to (aVector / aScalar)
template <class T>
void operator/=(Vector3<T>& aVector,const T& aScalar)
{
	aVector.x = aVector.x / aScalar;
	aVector.y = aVector.y / aScalar;
	aVector.z = aVector.z / aScalar;
}

template<class T>
inline Vector3<T>& Vector3<T>::operator=(const T& aTypeT)
{
	x,y,z = aTypeT;
	return *this;
}
#pragma endregion


template<class T>
inline Vector3<T> Vector3<T>::operator-() const
{
	return Vector3<T>(-x,-y,-z);
}

template<class T>
inline T Vector3<T>::LengthSqr() const
{
	return static_cast<T>(x * x + y * y + z * z);
}

template<class T>
inline T Vector3<T>::Length() const
{
	return static_cast<T>(std::sqrt(LengthSqr()));
}

template<class T>
inline Vector3<T> Vector3<T>::GetNormalized() const
{
	const T len = 1 / Length();

	if (len != len || isinf(len)) //IEEE FLOAT NAN
	{
		return Vector3<T>(0,0,0);
	}

	return Vector3<T>(
		x * len,
		y * len,
		z * len
	);
}


template<class T>
inline void Vector3<T>::Normalize()
{
	const T len = 1 / Length();

	if (len != len || isinf(len)) //IEEE FLOAT NAN
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

template<class T>
inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
{
	return  x * aVector.x + y * aVector.y + z * aVector.z;
}

template<class T>
inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
{
	return Vector3<T>(
		y * aVector.z - aVector.y * z,
		z * aVector.x - aVector.z * x,
		x * aVector.y - aVector.x * y
	);
}

template<class T>
inline Vector3<T>::Vector3()
{
	x = 0;
	y = 0;
	z = 0;
}

template<class T>
inline Vector3<T>::Vector3(const T& aX,const T& aY,const T& aZ)
{
	x = aX;
	y = aY;
	z = aZ;
}

template<class T>
inline Vector3<T>::Vector3(const T& aValue)
{
	x = aValue;
	y = aValue;
	z = aValue;
}

template<class T>
template<class U>
inline Vector3<T>::operator U() const
{
	return  { x,y,z };
}

template<class T>
inline T& Vector3<T>::operator [](int value)
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
std::ostream& operator<<(std::ostream& anOstream,const Vector3<T>& aVector)
{
	anOstream << "{x: " << aVector.x << ", y: " << aVector.y << ", z: " << aVector.z << "}";
	return anOstream;
}

template<class T>
inline const T& Vector3<T>::operator[](int value) const
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
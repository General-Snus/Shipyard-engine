#pragma once
#include <algorithm>
#include <assert.h>
#include <cassert>
#include <cmath>
#include "Vector3.hpp"
#include "VectorX.hpp"

template <class T>
class Vector4 {
public:
	T x;
	T y;
	T z;
	T w;
	// Creates a null-vector
	Vector4<T>();
	// Creates a vector (aX, aY, aZ)
	Vector4<T>(const T& aX,const T& aY,const T& aZ,const T& aW);
	Vector4<T>(const Vector3<T>& aVector,const T& aW);
	Vector4<T>(const VectorX<T,4>& aVector);

	// Copy constructor (compiler generated)
	Vector4<T>(const Vector4<T>& aVector) = default;
	// Assignment operator (compiler generated)
	Vector4<T>& operator=(const Vector4<T>& aVector4) = default;
	Vector4<T>& operator=(const T& aTypeT);
	// Destructor (compiler generated)
	~Vector4<T>() = default;


	static constexpr std::size_t size() { return {4}; };

	T* begin() { return &x; };
	T* end() { return &x + size(); };
	T* begin() const { return &x; };
	T* end() const { return &x + size(); };

	operator std::initializer_list<T>() const {
		return {x, y, z, w};
	}

	operator std::array<T,4>() const {
		return {x, y, z, w};
	}

	// Implicit cast operator to any datatype, return initializer list of x, y, z
	template <class U>
	operator U() const;

	// Returns the negated vector
	Vector4<T> operator-() const;

	// Returns the array vector
	T& operator[](int value);
	const T& operator[](int value) const;

	// Returns the squared length of the vector
	T LengthSqr() const;

	// Returns the length of the vector
	T Length() const;

	// Returns a normalized copy of this
	Vector4<T> GetNormalized() const;

	// Normalizes the vector
	void Normalize();
	// Returns the dot product of this and aVector
	T Dot(const Vector4<T>& aVector) const;

	Vector3<T> xyz() const;
};

template <class T>
Vector4<T> MinVector(const Vector4<T>& a,const Vector4<T>& b) {
	return Vector4<T>((a < b.x) ? a.x : b.x,(a < b.y) ? a.y : b.y,(a < b.z) ? a.z : b.z,(a < b.w) ? a.w : b.w);
}

template <class T>
Vector4<T> MaxVector(const Vector4<T>& a,const Vector4<T>& b) {
	return Vector4<T>((a > b.x) ? a.x : b.x,(a > b.y) ? a.y : b.y,(a > b.z) ? a.z : b.z,(a > b.w) ? a.w : b.w);
}

// Returns the vector sum of aVector0 and aVector1
template <class T>
Vector4<T> operator+(const Vector4<T>& aVector0,const Vector4<T>& aVector1) {
	return Vector4<T>(aVector0.x + aVector1.x,aVector0.y + aVector1.y,aVector0.z + aVector1.z,
		aVector0.w + aVector1.w);
}




// Returns the vector difference of aVector0 and aVector1
template <class T>
Vector4<T> operator-(const Vector4<T>& aVector0,const Vector4<T>& aVector1) {
	return Vector4<T>(aVector0.x - aVector1.x,aVector0.y - aVector1.y,aVector0.z - aVector1.z,
		aVector0.w - aVector1.w);
}

// Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector4<T> operator*(const Vector4<T>& aVector,const T& aScalar) {
	return Vector4<T>(aVector.x * aScalar,aVector.y * aScalar,aVector.z * aScalar,aVector.w * aScalar);
}

// Returns the vector aVector multiplied by the scalar aScalar
template <class T>
Vector4<T> operator*(const T& aScalar,const Vector4<T>& aVector) {
	return Vector4<T>(aVector.x * aScalar,aVector.y * aScalar,aVector.z * aScalar,aVector.w * aScalar);
}

template <class T>
Vector4<T> operator*(const Vector4<T>& lhf,const Vector4<T>& rhf) {
	return Vector4<T>(lhf.x * rhf.x,lhf.y * rhf.y,lhf.z * rhf.z,lhf.w * rhf.w);
}

// Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1 / aScalar)
template <class T>
Vector4<T> operator/(const Vector4<T>& aVector,const T& aScalar) {
	const T mlt = (1 / aScalar);

	return Vector4<T>(aVector.x * mlt,aVector.y * mlt,aVector.z * mlt,aVector.w * mlt);
}

// Equivalent to setting aVector0 to (aVector0 + aVector1)
template <class T>
void operator+=(Vector4<T>& aVector0,const Vector4<T>& aVector1) {
	aVector0.x = aVector0.x + aVector1.x;
	aVector0.y = aVector0.y + aVector1.y;
	aVector0.z = aVector0.z + aVector1.z;
	aVector0.w = aVector0.w + aVector1.w;
}

// Equivalent to setting aVector0 to (aVector0 - aVector1)
template <class T>
void operator-=(Vector4<T>& aVector0,const Vector4<T>& aVector1) {
	aVector0.x = aVector0.x - aVector1.x;
	aVector0.y = aVector0.y - aVector1.y;
	aVector0.z = aVector0.z - aVector1.z;
	aVector0.w = aVector0.w - aVector1.w;
}

// Equivalent to setting aVector to (aVector * aScalar)
template <class T>
void operator*=(Vector4<T>& aVector,const T& aScalar) {
	aVector.x = aVector.x * aScalar;
	aVector.y = aVector.y * aScalar;
	aVector.z = aVector.z * aScalar;
	aVector.w = aVector.w * aScalar;
}

// Equivalent to setting aVector to (aVector / aScalar)
template <class T>
void operator/=(Vector4<T>& aVector,const T& aScalar) {
	aVector.x = aVector.x / aScalar;
	aVector.y = aVector.y / aScalar;
	aVector.z = aVector.z / aScalar;
	aVector.w = aVector.w / aScalar;
}


template <class T>
Vector4<T>& Vector4<T>::operator=(const T& aTypeT) {
	x,y,z,w = aTypeT;
	return *this;
}

template <class T>
Vector4<T> Vector4<T>::operator-() const {
	return Vector4<T>(-x,-y,-z,-w);
}

template <class T>
T Vector4<T>::LengthSqr() const {
	return static_cast<T>(x * x + y * y + z * z + w * w);
}

template <class T>
T Vector4<T>::Length() const {
	return static_cast<T>(sqrt(LengthSqr()));
}

template <class T>
Vector4<T> Vector4<T>::GetNormalized() const {
	if(x == T(0) && y == T(0) && z == T(0) && w == T(0)) {
		return Vector4<T>(T(0),T(0),T(0),T(0));
	}
	const T len = 1 / Length();
	return Vector4<T>(x * len,y * len,z * len,w * len);
}

template <class T>
void Vector4<T>::Normalize() {
	const T len = 1 / Length();

	x = x * len;
	y = y * len;
	z = z * len;
	w = w * len;
}

template <class T>
T Vector4<T>::Dot(const Vector4<T>& aVector) const {
	return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
}

template<class T>
inline Vector3<T> Vector4<T>::xyz() const {
	return Vector3<T>(x,y,z);
}

template <class T>
Vector4<T>::Vector4() {
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

template <class T>
Vector4<T>::Vector4(const T& aX,const T& aY,const T& aZ,const T& aW) : x(aX),y(aY),z(aZ),w(aW) {}

template <class T>
Vector4<T>::Vector4(const Vector3<T>& aVector,const T& aW) : x(aVector.x),y(aVector.y),z(aVector.z),w(aW) {}

template <class T>
Vector4<T>::Vector4(const VectorX<T,4>& aVector) : x(aVector[0]),y(aVector[1]),z(aVector[2]),w(aVector[3]) {}

template <class T>
template <class U>
Vector4<T>::operator U() const {
	return {x, y, z, w};
}

template <class T>
T& Vector4<T>::operator[](int value) {
	assert(value >= 0 && value <= 4);
	switch(value) {
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
	case 3:
	{
		return w;
	}
	default:
	{
		return x;
	}
	}
}

template <class T>
const T& Vector4<T>::operator[](int value) const {
	assert(value >= 0 && value <= 4);
	switch(value) {
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
	case 3:
	{
		return w;
	}
	default:
	{
		return x;
	}
	}
}

using Vector4f = Vector4<float>;
using Vector4i = Vector4<int>;
using Vector4ui = Vector4<unsigned int>;

template <>
struct std::hash<Vector4f> {
	std::size_t operator()(const Vector4f& vector) const noexcept {
		std::size_t                seed = 0;
		constexpr std::hash<float> hasher;

		seed ^= hasher(vector.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hasher(vector.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hasher(vector.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hasher(vector.w) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

		return seed;
	}
};

#pragma once 
#include <iostream>
#include "../Math.hpp"
#include "Matrix3x3.hpp"
#include "Matrix4x4.h"
#include "Vectors.hpp"

template <typename T = float>
class Quaternion
{
public:
	Quaternion<T>(Vector3<T> aPitchYawRoll);
	Quaternion<T>(Vector4<T> rotation);
	Quaternion<T>(Matrix4x4<T>& rotMatrix);
	Quaternion<T>(Matrix3x3<T>& rotMatrix);
	Quaternion<T>();
	~Quaternion<T>();

	//copy
	Quaternion<T>(const Quaternion<T>& Q);
	Quaternion<T>& operator=(const Quaternion<T>& Q);

	template <class U> operator U() const;

	T* operator&();

	//Base functions
	Quaternion<T> operator-();
	Quaternion<T> Conjugate();
	T GetNormalizedSquared();
	void Normalize();
	Quaternion<T> GetNormalized();

	//Usability functions
	void RotateAroundX(T aAngle);
	void RotateAroundY(T aAngle);
	void RotateAroundZ(T aAngle);
	void RotateAroundAxis(T aAngle,Vector3<T> aAxis);



	Vector3f GetEulerAngles() const;
	void SetEulerAngles(const Vector3<T>& aRotation);

	Matrix3x3<T> GetRotationAs3x3() const;
	Matrix4x4<T> GetRotationAs4x4() const;

	void SetRotation(const Matrix3x3<T>& aRotation);
	void SetRotation(const Matrix4x4<T>& aRotation);
	static Quaternion RotationFromTo(const Vector3f& aFrom,const Vector3f& aTo);

	Vector3<T> GetForward();
	Vector3<T> GetUp();
	Vector3<T> GetRight();

	/*
	T Magnitude();
	Quaternion<T> GetNormalized();
	void Normalize();
	*/


	T x;
	T y;
	T z;
	T w;
};

template<typename T>
inline Quaternion<T>::Quaternion(Vector3<T> aPitchYawRoll)
{
	SetEulerAngles(aPitchYawRoll);
}

template<typename T>
inline Quaternion<T>::Quaternion(Vector4<T> rotation) : x(rotation.x),y(rotation.y),z(rotation.z),w(rotation.w)
{
}

template<typename T>
inline Quaternion<T>::Quaternion() : x(0),y(0),z(0),w(1)
{
}

template<typename T>
inline Quaternion<T>::Quaternion(Matrix4x4<T>& rotMatrix) : Quaternion<T>(Matrix3x3<T>(rotMatrix))
{
}

template<typename T>
inline Quaternion<T>::Quaternion(Matrix3x3<T>& aRotationMatrix)
{
	const float fourWSquaredMinus1 = aRotationMatrix(1,1) + aRotationMatrix(2,2) + aRotationMatrix(3,3);
	const float fourXSquaredMinus1 = aRotationMatrix(1,1) - aRotationMatrix(2,2) - aRotationMatrix(3,3);
	const float fourYSquaredMinus1 = aRotationMatrix(2,2) - aRotationMatrix(1,1) - aRotationMatrix(3,3);
	const float fourZSquaredMinus1 = aRotationMatrix(3,3) - aRotationMatrix(1,1) - aRotationMatrix(2,2);

	int biggestDickestIndexest = 0;
	float fourBiggestSquaredMinus1 = fourWSquaredMinus1;

	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestDickestIndexest = 1;
	}

	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestDickestIndexest = 2;
	}

	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestDickestIndexest = 3;
	}

	const float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
	const float mult = 0.25f / biggestVal;

	switch (biggestDickestIndexest)
	{
	case 0:
		w = biggestVal;
		x = (aRotationMatrix(2,3) - aRotationMatrix(3,2)) * mult;
		y = (aRotationMatrix(3,1) - aRotationMatrix(1,3)) * mult;
		z = (aRotationMatrix(1,2) - aRotationMatrix(2,1)) * mult;
		break;
	case 1:
		x = biggestVal;
		w = (aRotationMatrix(2,3) - aRotationMatrix(3,2)) * mult;
		y = (aRotationMatrix(1,2) + aRotationMatrix(2,1)) * mult;
		z = (aRotationMatrix(3,1) + aRotationMatrix(1,3)) * mult;
		break;
	case 2:
		y = biggestVal;
		w = (aRotationMatrix(3,1) - aRotationMatrix(1,3)) * mult;
		x = (aRotationMatrix(1,2) + aRotationMatrix(2,1)) * mult;
		z = (aRotationMatrix(2,3) + aRotationMatrix(3,2)) * mult;
		break;
	case 3:
		z = biggestVal;
		w = (aRotationMatrix(1,2) - aRotationMatrix(2,1)) * mult;
		x = (aRotationMatrix(3,1) + aRotationMatrix(1,3)) * mult;
		y = (aRotationMatrix(2,3) + aRotationMatrix(3,2)) * mult;
		break;
	default:
		w = T(1);
		x = T(0);
		y = T(0);
		z = T(0);
		break;
	}
}

template<typename T>
inline Quaternion<T>::~Quaternion()
{
}

template<typename T>
inline Quaternion<T>::Quaternion(const Quaternion<T>& Q)
{
	x = Q.x;
	y = Q.y;
	z = Q.z;
	w = Q.w;
}

template <typename T>
template <class U>
Quaternion<T>::operator U() const
{
	return { x,y,z ,w };
}

template <typename T>
T* Quaternion<T>::operator&()
{
	return &x;
}

template<typename T>
inline Vector3f Quaternion<T>::GetEulerAngles() const
{
	const float sqw = w * w;
	const float sqx = x * x;
	const float sqy = y * y;
	const float sqz = z * z;
	const float test = 2.0f * (y * w - x * z);
	Vector3f euler;
	if (test > .9999f)
	{
		// heading = rotation about z-axis
		euler.z = (-2.0f * std::atan2f(x,w));
		// bank = rotation about x-axis
		euler.x = 0;
		// attitude = rotation about y-axis
		euler.y = PIHALF;
	}
	else if (test == -1.0)
	{
		// heading = rotation about z-axis
		euler.z = (2.0f * std::atan2f(x,w));
		// bank = rotation about x-axis
		euler.x = 0;
		// attitude = rotation about y-axis
		euler.y = -PIHALF;
	}
	else
	{
		// heading = rotation about z-axis
		euler.z = std::atan2f(2.0f * (x * y + z * w),(sqx - sqy - sqz + sqw));
		// bank = rotation about x-axis
		euler.x = std::atan2f(2.0f * (y * z + x * w),(-sqx - sqy + sqz + sqw));
		// attitude = rotation about y-axis
		euler.y = std::asinf(std::clamp<float>(test,-1.0f,1.0f));
	}
	return euler;
}

template<typename T>
inline void Quaternion<T>::SetEulerAngles(const Vector3<T>& aRotationInDegrees)
{
	const Vector3<T> aPitchYawRoll = aRotationInDegrees * DEG_TO_RAD;
	const T c0 = std::cos(aPitchYawRoll[0] / 2);
	const T s0 = std::sin(aPitchYawRoll[0] / 2);
	const T c1 = std::cos(aPitchYawRoll[1] / 2);
	const T s1 = std::sin(aPitchYawRoll[1] / 2);
	const T c2 = std::cos(aPitchYawRoll[2] / 2);
	const T s2 = std::sin(aPitchYawRoll[2] / 2);

	const T c0c1 = c0 * c1;
	const T s0s1 = s0 * s1;
	const T s0c1 = s0 * c1;
	const T c0s1 = c0 * s1;

	w = c0c1 * c2 + s0s1 * s2;
	x = s0c1 * c2 - c0c1 * s2;
	y = c0s1 * c2 + s0c1 * s2;
	z = c0c1 * s2 - s0s1 * c2;
};

template<typename T>
inline Quaternion<T>& Quaternion<T>::operator=(const Quaternion<T>& Q)
{
	x = Q.x;
	y = Q.y;
	z = Q.z;
	w = Q.w;
	return *this;
}

template<typename T>
inline Matrix3x3<T> Quaternion<T>::GetRotationAs3x3() const
{
	Matrix3x3<T> mat;
	const T xy = x * y;
	const T xz = x * z;

	const T wx = w * x;
	const T wy = w * y;
	const T wz = w * z;

	const T yz = y * z;

	const T x2 = x * x;
	const T y2 = y * y;
	const T z2 = z * z;

	mat(1,1) = 1 - 2 * (y2 + z2);
	mat(1,2) = 2 * (xy + wz);
	mat(1,3) = 2 * (xz - wy);

	mat(2,1) = 2 * (xy - wz);
	mat(2,2) = 1 - 2 * (x2 + z2);
	mat(2,3) = 2 * (yz + wx);

	mat(3,1) = 2 * (xz + wy);
	mat(3,2) = 2 * (yz - wx);
	mat(3,3) = 1 - 2 * (5 + 2);

	return mat;
}

template<typename T>
inline Matrix4x4<T> Quaternion<T>::GetRotationAs4x4() const
{
	Matrix4x4<T> mat;
	Matrix3x3<T> mat3x3;


	mat(1,1) = mat3x3(1,1);
	mat(1,2) = mat3x3(1,2);
	mat(1,3) = mat3x3(1,3);

	mat(2,1) = mat3x3(2,1);
	mat(2,2) = mat3x3(2,2);
	mat(2,3) = mat3x3(2,3);

	mat(3,1) = mat3x3(3,1);
	mat(3,2) = mat3x3(3,2);
	mat(3,3) = mat3x3(3,3);

	return mat;
}

template<typename T>
inline void Quaternion<T>::SetRotation(const Matrix3x3<T>& m)
{
	float fourWSquaredMinus1 = m(1,1) + m(2,2) + m(3,3);
	float fourXSquaredMinus1 = m(1,1) - m(2,2) - m(3,3);
	float fourYSquaredMinus1 = m(2,2) - m(1,1) - m(3,3);
	float fourZSquaredMinus1 = m(3,3) - m(1,1) - m(2,2);

	int biggestIndex = 0;
	float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	float biggestVal = std::sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
	float mult = 0.25f / biggestVal;

	switch (biggestIndex)
	{
	case 0:
		w = biggestVal;
		x = (m(2,3) - m(3,2)) * mult;
		y = (m(3,1) - m(1,3)) * mult;
		z = (m(1,2) - m(2,1)) * mult;
		break;

	case 1:
		x = biggestVal;
		w = (m(2,3) - m(3,2)) * mult;
		y = (m(1,2) + m(2,1)) * mult;
		z = (m(3,1) + m(1,3)) * mult;
		break;

	case 2:
		y = biggestVal;
		w = (m(3,1) - m(1,3)) * mult;
		x = (m(1,2) + m(2,1)) * mult;
		z = (m(2,3) + m(3,2)) * mult;
		break;

	case 3:
		z = biggestVal;
		w = (m(1,2) - m(2,1)) * mult;
		x = (m(3,1) + m(1,3)) * mult;
		y = (m(2,3) + m(3,2)) * mult;
		break;
	}

}

template<typename T>
inline Quaternion<T> Quaternion<T>::RotationFromTo(const Vector3f& aFrom,const Vector3f& aTo)
{
	Vector3f v0 = aFrom;
	Vector3f v1 = aTo;
	v0.Normalize();
	v1.Normalize();

	const T d = v0.Dot(v1);
	if (d >= 1.0f) // If dot == 1, vectors are the same
	{
		return Quaternion<T>();
	}
	else if (d <= -1.0f) // exactly opposite
	{
		Vector3f axis(1.0f,0.f,0.f);
		axis = axis.Cross(v0);
		if (axis.Length() == 0)
		{
			axis = Vector3f(0.f,1.f,0.f);
			axis = axis.Cross(v0);
		}
		// same as fromAngleAxis(core::PI, axis).normalize();
		return Quaternion<float>(Vector4f(axis.x,axis.y,axis.z,0));
	}

	const float s = std::sqrtf((1 + d) * 2);
	const float invs = 1.f / s;
	const Vector3f c = v0.Cross(v1) * invs;


	auto out = Quaternion<float>(Vector4f(c.x,c.y,c.z,s * 0.5f));
	out.Normalize();
	return out;
}

template<typename T>
inline T Quaternion<T>::GetNormalizedSquared()
{
	return T();
}

template<typename T>
inline Vector3<T> Quaternion<T>::GetForward()
{
	return Vector3<T>(
		2 * (x * z + w * y),
		2 * (y * z - w * x),
		1 - 2 * (x * x + y * y)
	).GetNormalized();
}
template<typename T>
inline Vector3<T> Quaternion<T>::GetUp()
{
	return Vector3<T>(
		2 * (x * y - w * z),
		1 - 2 * (x * x + z * z),
		2 * (y * z + w * x)
	).GetNormalized();
}
template<typename T>
inline Vector3<T> Quaternion<T>::GetRight()
{
	return -Vector3<T>(
		1 - 2 * (y * y + z * z),
		2 * (x * y + w * z),
		2 * (x * z - w * y)
	).GetNormalized();
}
template<typename T>
inline void Quaternion<T>::Normalize()
{
	const float n = x * x + y * y + z * z + w * w;

	if (n == 1)
	{
		return;
	}
	*this = (*this * (1 / sqrtf(n)));
}

template<typename T>
inline Quaternion<T> Quaternion<T>::GetNormalized()
{
	const float n = x * x + y * y + z * z + w * w;

	if (n == 1)
	{
		return *this;
	}
	return Quaternion<T>(*this * (T(1) / sqrtf(n)));
}

#pragma region Operators
template<typename T>
inline Quaternion<T> operator*(const Quaternion<T>& quat,const T value)
{
	return Quaternion<T>(Vector4f(quat.x * value,quat.y * value,quat.z * value,quat.w * value));
}
#pragma endregion

using Quaternionf = Quaternion<float>;
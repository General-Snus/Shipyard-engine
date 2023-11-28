#pragma once 
#include "Vectors.hpp"
#include "Matrix4x4.hpp"
#include "Matrix3x3.hpp"
#include <iostream>
#include "../Math.hpp"

template <typename T = float>
class Quaternion
{
public:
	Quaternion<T>(Vector3f rotation);
	Quaternion<T>(Vector4f rotation);
	Quaternion<T>();
	~Quaternion<T>();

	//copy
	Quaternion<T>(const Quaternion<T>& Q);
	Quaternion<T>& operator=(const Quaternion<T>& Q);


	//Base functions
	Quaternion<T> operator-();
	Quaternion<T> Conjugate();
	T GetNormalizedSquared();
	void Normalize();

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
inline Quaternion<T>::Quaternion(Vector3f rotation) : x(rotation.x),y(rotation.y),z(rotation.z),w(1)
{
}
template<typename T>
inline Quaternion<T>::Quaternion(Vector4f rotation) : x(rotation.x),y(rotation.y),z(rotation.z),w(rotation.w)
{
}
template<typename T>
inline Quaternion<T>::Quaternion() : x(0),y(0),z(0),w(1)
{
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

template<typename T>
inline Vector3f Quaternion<T>::GetEulerAngles() const
{
	const float sqw = w * w;
	const float sqx = x * x;
	const float sqy = y * y;
	const float sqz = z * z;
	const float test = 2.0f * (y * w - x * z);
	Vector3f euler;
	if(test == 1.0)
	{
		// heading = rotation about z-axis
		euler.z = (-2.0f * std::atan2f(x,w));
		// bank = rotation about x-axis
		euler.x = 0;
		// attitude = rotation about y-axis
		euler.y = PIHALF;
	}
	else if(test == -1.0)
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
	if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	float biggestVal = std::sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
	float mult = 0.25f / biggestVal;

	switch(biggestIndex)
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
	if(d >= 1.0f) // If dot == 1, vectors are the same
	{
		return Quaternion<T>();
	}
	else if(d <= -1.0f) // exactly opposite
	{
		Vector3f axis(1.0f,0.f,0.f);
		axis = axis.Cross(v0);
		if(axis.Length() == 0)
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
inline void Quaternion<T>::Normalize()
{
	const float n = x * x + y * y + z * z + w * w;

	if(n == 1)
	{
		return;
	}
	*this = (*this * (1 / sqrtf(n)));
}

#pragma region Operators
template<typename T>
inline Quaternion<T> operator*(const Quaternion<T>& quat,const T value)
{
	return Quaternion<T>(Vector4f(quat.x * value,quat.y * value,quat.z * value,quat.w * value));
}
#pragma endregion

using Quaternionf = Quaternion<float>;
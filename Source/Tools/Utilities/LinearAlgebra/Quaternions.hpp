#pragma once
#include <iostream>
#include <cmath>
#include <utility>
#include "Matrix3x3.hpp"
#include "Matrix4x4.h"
#include "Vectors.hpp" 
#include "../Math.hpp" 

template <typename T = float>
class Quaternion {
public:
	Quaternion<T>(const Vector3<T> aPitchYawRoll);
	Quaternion<T>(const Vector4<T> rotation);
	Quaternion<T>(const Matrix4x4<T>& rotMatrix);
	Quaternion<T>(const Matrix3x3<T>& rotMatrix);
	Quaternion<T>(const T x,const T y,const T z,const T w);
	Quaternion<T>();
	~Quaternion<T>();

	// copy
	Quaternion<T>(const Quaternion<T>& Q);
	Quaternion<T>& operator=(const Quaternion<T>& Q);

	template <class U>
	operator U() const;

	T* operator&();

	// Base functions
	Quaternion<T> operator-();
	bool IsClose(const Quaternion<T>& other,T epsilon = 1e-5) const;

	// Usability functions
	void RotateAroundX(T aAngle);
	void RotateAroundY(T aAngle);
	void RotateAroundZ(T aAngle);
	void RotateAroundAxis(T aAngle,Vector3<T> aAxis);

	Vector3<T> GetEulerAngles() const;
	void     SetEulerAngles(const Vector3<T>& aRotation);

	Matrix3x3<T> GetRotationAs3x3();
	Matrix4x4<T> GetRotationAs4x4();

	void                 SetRotation(const Matrix3x3<T>& aRotation);
	void                 SetRotation(const Matrix4x4<T>& aRotation);

	//Statics
	static Quaternion    RotationFromTo(const Vector3f& aFrom,const Vector3f& aTo);
	static Quaternion<T> LookAt(const Vector3<T>& source,const Vector3<T>& point,const Vector3f& front,
		const Vector3f& up);

	static Quaternion<T> LookAt(const Vector2<T>& direction,const Vector3f& up);
	static Quaternion<T> LookAt(const Vector3<T>& direction,const Vector3f& up);
	static Quaternion<T> CreateFromAxisAngle(Vector3<T> axis,T angle);
	static T Dot(const Quaternion<T>& q1,const Quaternion<T>& q2);
	static Quaternion<T> GetInverse(const Quaternion<T>& q1);
	static T GetMagnitudeSquared(const Quaternion<T>& q1);
	static Quaternion<T> GetNormalized(const Quaternion<T>& q1);
	static Quaternion<T> Pow(const Quaternion<T>& q1,T t);
	static Quaternion<T> Slerp(const Quaternion<T>& q1,const Quaternion<T>& q2,float decimal);
	static Quaternion<T> Conjugate(const Quaternion<T>& q1);
	constexpr static Quaternion<T> Identity();

	//Instance
	T Dot(const Quaternion<T>& q1);
	Quaternion<T> GetInverse();
	void Inverse();
	T GetMagnitudeSquared();
	void Normalize();
	Quaternion<T> GetNormalized();
	Quaternion<T> Pow(T t);
	Quaternion<T> Slerp(const Quaternion<T>& q1,float decimal);
	Quaternion<T> GetConjugate(); 

	Vector3<T>    GetForward();
	Vector3<T>    GetUp();
	Vector3<T>    GetRight();

	/*
	T Magnitude();
	Quaternion<T> GetNormalized();
	void Normalize();
	*/
#pragma warning( push )
#pragma warning( disable : 4201)  
	union {
		struct {
			T x;
			T y;
			T z;
			T w;
		};
		struct {
			Vector3<T> xyz;
			T          w;
		};

		struct {
			Vector4<T> xyzw;
		};
	};
#pragma warning( pop ) 
};

template <typename T>
Quaternion<T>::Quaternion(const Vector3<T> aPitchYawRoll) {
	SetEulerAngles(aPitchYawRoll);
}

template <typename T>
Quaternion<T>::Quaternion(const Vector4<T> rotation) : x(rotation.x),y(rotation.y),z(rotation.z),w(rotation.w) {}

template <typename T>
Quaternion<T>::Quaternion() : x(0),y(0),z(0),w(1) {}

template <typename T>
Quaternion<T>::Quaternion(const Matrix4x4<T>& rotMatrix) : Quaternion<T>(Matrix3x3<T>(rotMatrix.rotationMatrix())) {}

template <typename T>
Quaternion<T>::Quaternion(const Matrix3x3<T>& aRotationMatrix) {

	const auto m00 = aRotationMatrix(1,1);
	const auto m11 = aRotationMatrix(2,2);
	const auto m22 = aRotationMatrix(3,3);
	const auto m21 = aRotationMatrix(3,2);
	const auto m12 = aRotationMatrix(2,3);
	const auto m02 = aRotationMatrix(1,3);
	const auto m20 = aRotationMatrix(3,1);
	const auto m10 = aRotationMatrix(2,1);
	const auto m01 = aRotationMatrix(1,2);
	 
	float r22 = m22;
	if(r22 <= 0.f)  // x^2 + y^2 >= z^2 + w^2
	{
		float dif10 = m11 - m00;
		float omr22 = 1.f - r22;
		if(dif10 <= 0.f)  // x^2 >= y^2
		{
			float fourXSqr = omr22 - dif10;
			float inv4x = 0.5f / sqrtf(fourXSqr);
			x = fourXSqr * inv4x;
			y = (m01 + m10) * inv4x;
			z = (m02 + m20) * inv4x;
			w = (m12 - m21) * inv4x;
		} else  // y^2 >= x^2
		{
			float fourYSqr = omr22 + dif10;
			float inv4y = 0.5f / sqrtf(fourYSqr);
			x = (m01 + m10) * inv4y;
			y = fourYSqr * inv4y;
			z = (m12 + m21) * inv4y;
			w = (m20 - m02) * inv4y;
		}
	} else  // z^2 + w^2 >= x^2 + y^2
	{
		float sum10 = m11 + m00;
		float opr22 = 1.f + r22;
		if(sum10 <= 0.f)  // z^2 >= w^2
		{
			float fourZSqr = opr22 - sum10;
			float inv4z = 0.5f / sqrtf(fourZSqr);
			x = (m02 + m20) * inv4z;
			y = (m12 + m21) * inv4z;
			z = fourZSqr * inv4z;
			w = (m01 - m10) * inv4z;
		} else  // w^2 >= z^2
		{
			float fourWSqr = opr22 + sum10;
			float inv4w = 0.5f / sqrtf(fourWSqr);
			x = (m12 - m21) * inv4w;
			y = (m20 - m02) * inv4w;
			z = (m01 - m10) * inv4w;
			w = fourWSqr * inv4w;
		}
	}

	//w = std::sqrt(std::max(T(0),1 + m00 + m11 + m22)) / 2;
	//x = std::sqrt(std::max(T(0),1 + m00 - m11 - m22)) / 2;
	//y = std::sqrt(std::max(T(0),1 - m00 + m11 - m22)) / 2;
	//z = std::sqrt(std::max(T(0),1 - m00 - m11 + m22)) / 2;
	//x = std::copysign(x,m21 - m12);
	//y = std::copysign(y,m02 - m20);
	//z = std::copysign(z,m10 - m01);
}

template <typename T>
Quaternion<T>::Quaternion(const T x,const T y,const T z,const T w) : x(x),y(y),z(z),w(w) {}

template <typename T>
Quaternion<T>::~Quaternion() = default;

template <typename T>
Quaternion<T>::Quaternion(const Quaternion<T>& Q) {
	x = Q.x;
	y = Q.y;
	z = Q.z;
	w = Q.w;
}

template <typename T>
template <class U>
Quaternion<T>::operator U() const {
	return {x, y, z, w};
}

template <typename T>
T* Quaternion<T>::operator&() {
	return &x;
}

//Unoptimized on purpose to return clearer angle types
template <typename T>
Vector3<T> Quaternion<T>::GetEulerAngles() const {

	auto eulerToReadable = [](const Vector3<T>& arg) -> Vector3<T> {
		//Vector3<T> returnValue = arg;
		//returnValue.x += T(180.0);
		//returnValue.y += T(180.0);
		//returnValue.z += T(180.0);
		return arg;
		};

	const T xx = x * x;
	const T yy = y * y;
	const T zz = z * z;

	const T m31 = 2.f * x * z + 2.f * y * w;
	const T m32 = 2.f * y * z - 2.f * x * w;
	const T m33 = 1.f - 2.f * xx - 2.f * yy;

	const T cy = std::sqrt(m33 * m33 + m31 * m31);
	const T cx = std::atan2(-m32,cy);
	if(cy > T(16.f * FLT_EPSILON)) {
		const float m12 = 2.f * x * y + 2.f * z * w;
		const float m22 = 1.f - 2.f * xx - 2.f * zz;

		return eulerToReadable(Math::RAD_TO_DEG * Vector3<T>(cx,std::atan2(m31,m33),std::atan2(m12,m22)));
	} else {
		const T m11 = 1.f - 2.f * yy - 2.f * zz;
		const T m21 = 2.f * x * y - 2.f * z * w;

		return eulerToReadable(Math::RAD_TO_DEG * Vector3<T>(cx,0.f,std::atan2(-m21,m11)));
	}
}

template <typename T>
void Quaternion<T>::SetEulerAngles(const Vector3<T>& aRotationInDegrees) {
	const Vector3<T> aPitchYawRoll = aRotationInDegrees * Math::DEG_TO_RAD;
	


	const float halfpitch = aPitchYawRoll[0] * 0.5f;
	float cp = cosf(halfpitch);
	float sp = sinf(halfpitch);

	const float halfyaw = aPitchYawRoll[1] * 0.5f;
	float cy = cosf(halfyaw);
	float sy = sinf(halfyaw);

	const float halfroll = aPitchYawRoll[2] * 0.5f;
	float cr = cosf(halfroll);
	float sr = sinf(halfroll);

	x = cr * sp * cy + sr * cp * sy;
	y = cr * cp * sy - sr * sp * cy;
	z = sr * cp * cy - cr * sp * sy;
	w = cr * cp * cy + sr * sp * sy;
};

template <typename T>
Quaternion<T>& Quaternion<T>::operator=(const Quaternion<T>& Q) {
	x = Q.x;
	y = Q.y;
	z = Q.z;
	w = Q.w;
	return *this;
}

template <typename T>
Matrix3x3<T> Quaternion<T>::GetRotationAs3x3() {
	Normalize();

	Matrix3x3<T> mat;
	const float qx = x;
	const float qxx = qx * qx;

	const float qy = y;
	const float qyy = qy * qy;

	const float qz = z;
	const float qzz = qz * qz;

	const float qw = w;

	mat(1,1) = 1.f - 2.f * qyy - 2.f * qzz;
	mat(1,2) = 2.f * qx * qy + 2.f * qz * qw;
	mat(1,3) = 2.f * qx * qz - 2.f * qy * qw;
	mat(2,1) = 2.f * qx * qy - 2.f * qz * qw;
	mat(2,2) = 1.f - 2.f * qxx - 2.f * qzz;
	mat(2,3) = 2.f * qy * qz + 2.f * qx * qw;
	mat(3,1) = 2.f * qx * qz + 2.f * qy * qw;
	mat(3,2) = 2.f * qy * qz - 2.f * qx * qw;
	mat(3,3) = 1.f - 2.f * qxx - 2.f * qyy;

	return mat;
}

template <typename T>
Matrix4x4<T> Quaternion<T>::GetRotationAs4x4() {
	Matrix4x4<T> mat;
	Matrix3x3<T> mat3x3 = GetRotationAs3x3();

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

template <typename T>
void Quaternion<T>::SetRotation(const Matrix3x3<T>& m) {
	const float fourWSquaredMinus1 = m(1,1) + m(2,2) + m(3,3);
	const float fourXSquaredMinus1 = m(1,1) - m(2,2) - m(3,3);
	const float fourYSquaredMinus1 = m(2,2) - m(1,1) - m(3,3);
	const float fourZSquaredMinus1 = m(3,3) - m(1,1) - m(2,2);

	int   biggestIndex = 0;
	float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	if(fourXSquaredMinus1 > fourBiggestSquaredMinus1) {
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if(fourYSquaredMinus1 > fourBiggestSquaredMinus1) {
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if(fourZSquaredMinus1 > fourBiggestSquaredMinus1) {
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	float biggestVal = std::sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
	float mult = 0.25f / biggestVal;

	switch(biggestIndex) {
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
inline void Quaternion<T>::SetRotation(const Matrix4x4<T>& aRotation) { SetRotation(aRotation.rotationMatrix()); }

template <typename T>
Quaternion<T> Quaternion<T>::RotationFromTo(const Vector3f& aFrom,const Vector3f& aTo) {
	Vector3f from = aFrom;
	Vector3f to = aTo;
	from.Normalize();
	to.Normalize();

	const T d = from.Dot(to);
	if(d >= 1.0f) {
		return Quaternion<T>();
	}
	if(d <= -1.0f) {
		Vector3f axis(1.0f,0.f,0.f);
		axis = axis.Cross(from);
		if(axis.Length() == 0) {
			axis = Vector3f(0.f,1.f,0.f);
			axis = axis.Cross(from);
		}
		// same as fromAngleAxis(core::PI, axis).normalize();
		return Quaternion<float>(Vector4f(axis.x,axis.y,axis.z,0));
	}

	const float    s = std::sqrtf((1 + d) * 2);
	const float    invs = 1.f / s;
	const Vector3f c = from.Cross(to) * invs;

	auto out = Quaternion<float>(Vector4f(c.x,c.y,c.z,s * 0.5f));
	out.Normalize();
	return out;
}

template <typename T>
Quaternion<T> Quaternion<T>::LookAt(const Vector3<T>& source,const Vector3<T>& destinationPoint,const Vector3f& front,
	const Vector3f& up) {
	Vector3f toVector = (destinationPoint - source).GetNormalized();
	float dot = front.Dot(toVector);


	if(std::abs(dot + 1.0f) < 0.000001f) {
		return CreateFromAxisAngle(up,Math::PI);
	}
	if(std::abs(dot - 1.0f) < 0.000001f) {
		return Quaternion();
	}


	float rotAngle = std::acosf(dot);
	//Vector3 rotAxis = front.Cross(toVector);
	Vector3 rotAxis = toVector.Cross(front);
	rotAxis.Normalize();
	// convert axis angle to quaternion
	return CreateFromAxisAngle(rotAxis,rotAngle);
}


template <typename T>
Quaternion<T> Quaternion<T>::LookAt(const Vector2<T>& aDirection,const Vector3f& aUp) {
	return Quaternion<T>::LookAt(Vector3<T>(aDirection.x,0,aDirection.y),aUp);
}

template <typename T>
Quaternion<T> Quaternion<T>::LookAt(const Vector3<T>& aDirection,const Vector3f& aUp) {


	const Vector3f forward = -aDirection.GetNormalized();
	Vector3f up = aUp.GetNormalized();
	const Vector3f right = up.Cross(forward);
	up = forward.Cross(right);

	if(forward.IsNearlyEqual({0, 0, 1})) {
		return Quaternion<float>(); // Identity quaternion
	}
	if(up.IsNearlyEqual(forward)) {
		return Quaternion<float>(); // Identity quaternion
	}

	//from world forward to Object forward
	const Quaternion f2d = RotationFromTo({0,0,1},forward);

	//what direction is the new object up?
	const Vector3f objectUp = f2d * Vector3f(0,1,0);
	//from object up to desired up
	const Quaternion u2u = RotationFromTo(objectUp,up);

	//rotate to forward direction first, then twist to correct up
	return (u2u * f2d).GetNormalized();
}

template <typename T>
Quaternion<T> Quaternion<T>::CreateFromAxisAngle(Vector3<T> axis,T angle) {
	const float halfAngle = angle * .5f;
	const float s = std::sin(halfAngle);
	axis.Normalize();
	return Quaternion<T>(axis.x * s,axis.y * s,axis.z * s,std::cos(halfAngle));
}

template<typename T>
inline T Quaternion<T>::Dot(const Quaternion<T>& q1,const Quaternion<T>& q2) {
	return q1.xyzw.Dot(q2.xyzw);
}

template<typename T>
inline Quaternion<T> Quaternion<T>::GetInverse(const Quaternion<T>& q1) {

	const auto sqr = GetMagnitudeSquared(q1);

	if(sqr < Math::Epsilon) {
		return Quaternion<T>();
	}

	return Conjugate(q1) * (1 / sqr);
}

template<typename T>
T Quaternion<T>::GetMagnitudeSquared(const Quaternion<T>& q1) {
	return q1.x * q1.x + q1.y * q1.y + q1.z * q1.z + q1.w * q1.w;
}

template<typename T>
Quaternion<T> Quaternion<T>::GetNormalized(const Quaternion<T>& q1) {
	const float n = GetMagnitudeSquared(q1);

	if(std::abs(n - 1.0f) < Math::Epsilon) {
		return  q1;
	}

	const T invLen = (1.0f / sqrt(n));
	return Quaternion<T>(q1.x * invLen,q1.y * invLen,q1.z * invLen,q1.w * invLen);
}

template<typename T>
Quaternion<T> Quaternion<T>::Pow(const Quaternion<T>& q1,T t) {
	const Quaternion<T> q = GetNormalized(q1);

	const T angle = std::acos(q.w);
	const T sinAngle = std::sin(angle);

	if(sinAngle == 0.0) {
		return q1;
	}

	const Vector3<T> axis(q.x / sinAngle,q.y / sinAngle,q.z / sinAngle);
	const T newAngle = angle * t;

	const T newSinAngle = std::sin(newAngle);
	const Quaternion<T> result(
		axis.x * newSinAngle,
		axis.y * newSinAngle,
		axis.z * newSinAngle,
		std::cos(newAngle)
	);

	return result;
}

template<typename T>
inline Quaternion<T> Quaternion<T>::Slerp(const Quaternion<T>& q1,const Quaternion<T>& q2,float decimal) {
	return Pow(q2 * GetInverse(q1),decimal) * q1;
}

template<typename T>
constexpr Quaternion<T> Quaternion<T>::Identity() {
	return Quaternion<T>();
}

template<typename T>
inline T Quaternion<T>::Dot(const Quaternion<T>& q1) {
	return Dot(*this,q1);
}

template<typename T>
inline Quaternion<T> Quaternion<T>::GetInverse() {
	return GetInverse(*this);
}

template<typename T>
inline void Quaternion<T>::Inverse() {
	*this = GetInverse(*this);
}

template <typename T>
Quaternion<T> Quaternion<T>::Conjugate(const Quaternion<T>& q1) {
	return Quaternion<T>(-q1.x,-q1.y,-q1.z,q1.w);
}

template <typename T>
T Quaternion<T>::GetMagnitudeSquared() {
	return GetMagnitudeSquared(*this);
}

template <typename T>
void Quaternion<T>::Normalize() {
	*this = GetNormalized(*this);
}

template <typename T>
Quaternion<T> Quaternion<T>::GetNormalized() {
	return GetNormalized(*this);
}

template<typename T>
inline Quaternion<T> Quaternion<T>::Pow(T t) {
	return Quaternion<T>();
}

template<typename T>
inline Quaternion<T> Quaternion<T>::Slerp(const Quaternion<T>& q1,float decimal) {
	return Quaternion<T>();
}

template<typename T>
inline Quaternion<T> Quaternion<T>::GetConjugate() {
	return Conjugate(*this);
}

template <typename T>
Vector3<T> Quaternion<T>::GetForward() {
	return Vector3<T>(2 * (x * z + w * y),2 * (y * z - w * x),1 - 2 * (x * x + y * y)).GetNormalized();
}

template <typename T>
Vector3<T> Quaternion<T>::GetUp() {
	return Vector3<T>(2 * (x * y - w * z),1 - 2 * (x * x + z * z),2 * (y * z + w * x)).GetNormalized();
}

template <typename T>
Vector3<T> Quaternion<T>::GetRight() {
	return -Vector3<T>(1 - 2 * (y * y + z * z),2 * (x * y + w * z),2 * (x * z - w * y)).GetNormalized();
}

template <typename T>
bool Quaternion<T>::IsClose(const Quaternion<T>& other,T epsilon) const {
	return
		std::abs(x - other.x) < epsilon &&
		std::abs(y - other.y) < epsilon &&
		std::abs(z - other.z) < epsilon &&
		std::abs(w - other.w) < epsilon ||
		std::abs(x + other.x) < epsilon &&
		std::abs(y + other.y) < epsilon &&
		std::abs(z + other.z) < epsilon &&
		std::abs(w + other.w) < epsilon;
}

#pragma region Operators
template <typename T>
Quaternion<T> operator*(const Quaternion<T>& quat,const T value) {
	return Quaternion<T>(Vector4f(quat.x * value,quat.y * value,quat.z * value,quat.w * value));
}

template <typename T,typename T1>
Quaternion<T> operator*(const Quaternion<T>& x,T1 y) {
	return Quaternion<T>(x) *= y;
}

template <typename T>
Quaternion<T> operator*(const Quaternion<T>& lhf,const Quaternion<T>& rhf) {
	
	const Vector4<T> lwwww= Vector4<T>(lhf.w,lhf.w,lhf.w,lhf.w);

	const Vector4<T> rxyzw= Vector4<T>(rhf.x,rhf.y,rhf.z,rhf.w);

	const Vector4<T> lxyzx = Vector4<T>(lhf.x,lhf.y,lhf.z,lhf.x);

	const Vector4<T> rwwwx = Vector4<T>(rhf.w,rhf.w,rhf.w,rhf.x);

	const Vector4<T> lyzxy= Vector4<T>(lhf.y,lhf.z,lhf.x,lhf.y);

	const Vector4<T> realNegate = Vector4<T>(1.0,1.0,1.0,-1.0);

	const Vector4<T> rzxyy = Vector4<T>(rhf.z,rhf.x,rhf.y,rhf.y);

	const Vector4<T> lzxyz = Vector4<T>(lhf.z,lhf.x,lhf.y,lhf.z);

	const Vector4<T> ryzxz = Vector4<T>(rhf.y,rhf.z,rhf.x,rhf.z);
	
	return Quaternion<T>(lwwww * rxyzw + (lxyzx * rwwwx + lyzxy * rzxyy) * realNegate - lzxyz * ryzxz);

	//return
	//{
	//	lhf.w * rhf.x + lhf.x * rhf.w + lhf.y * rhf.z - lhf.z * rhf.y,
	//	lhf.w * rhf.y + lhf.y * rhf.w + lhf.z * rhf.x - lhf.x * rhf.z,
	//	lhf.w * rhf.z + lhf.z * rhf.w + lhf.x * rhf.y - lhf.y * rhf.x,
	//	lhf.w * rhf.w - lhf.x * rhf.x - lhf.y * rhf.y - lhf.z * rhf.z
	//};
}

template <typename T>
Vector3<T> operator*(const Quaternion<T>& aQuat,const Vector3<T>& v) {
	const Vector3<T> r = (aQuat.x,aQuat.y,aQuat.z);
	const T s = aQuat.w;

	return
		2.0f * r.Dot(v) * r
		+ (s * s - r.Dot(r)) * v
		+ 2.0f * s * r.Cross(v);
}

template <typename T,typename T1>
Quaternion<T>& operator*=(Quaternion<T>& lhf,const T1& rhf) {
	lhf = lhf * rhf;
	lhf.Normalize();
	return lhf;
}

#pragma endregion

using Quaternionf = Quaternion<float>;

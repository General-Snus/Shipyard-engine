#pragma once
#include <algorithm>
#include <cmath>
#include <ThirdParty/CU/CommonUtills/Vectors.hpp>

constexpr float PI = 3.14159f;
constexpr float PI2 = PI * 2.0f;
constexpr float PIHALF = PI * 0.5f;
constexpr float PIQUARTER = PI * 0.25f;

constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;
constexpr float Sqrt2 = 1.41421356237309504880f;

//Units
constexpr float Micro = 0.000001f;
constexpr float Milli = 0.001f;
constexpr float Centi = 0.01f;
constexpr float Deci = 0.1f;
constexpr float Kilo = 1000.0f;
constexpr float Mega = 1000000.0f;

//Bytes
constexpr float BitToByte = 1/8.f; 
constexpr float ByteToBit = 8.f; 


constexpr float Byte = 1.f;
constexpr float kByte = 1024.f;
constexpr float megByte = 1048576.f;
constexpr float gigByte = 1073741824.f;
constexpr float terByte = 1099511627776.f; 

//Bytes

const Vector3f GlobalRight = Vector3f(1.0,0.0,0.0);
const Vector3f GlobalUp = Vector3f(0.0,1.0,0.0);
const Vector3f GlobalFwd = Vector3f(0.0,0.0,1.0);




inline float Log2(float x)
{
	const float invLog2 = 1.442695040888963387004650940071f;
	return std::log(x) * invLog2;
}

template <typename T>
inline T Mod(T a,T b)
{
	T result = a - (a / b) * b;
	return (T)((result < 0) ? result + b : result);
}

template <>
inline float Mod(float a,float b)
{
	return std::fmod(a,b);
}
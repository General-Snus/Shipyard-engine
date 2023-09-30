#pragma once

const float PI = 3.14159f;
const float PI2 = PI * 2.0f;
const float PIHALF = PI * 0.5f;
const float PIQUARTER = PI * 0.25f;

const float DEG_TO_RAD = PI / 180.0f;
const float RAD_TO_DEG = 180.0f / PI;
const float Sqrt2 = 1.41421356237309504880f;

//Units
const float Micro = 0.000001f;
const float Milli = 0.001f;
const float Centi = 0.01f;
const float Deci = 0.1f;
const float Kilo = 1000.0f;
const float Mega = 1000000.0f; 

 












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
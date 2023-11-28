#pragma once
#include <algorithm>
#include <cmath>
#include "LinearAlgebra/Vectors.hpp"
#include <random>
#include <numbers>
#include <chrono> // For DefaultSeed generation
#include <Tools/Utilities/Game/Timer.h>

constexpr float PI = std::numbers::pi_v<float>;
constexpr float PI2 = PI * 2.0f;
constexpr float PIHALF = PI * 0.5f;
constexpr float PIQUARTER = PI * 0.25f;

constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;
constexpr float Sqrt2 = std::numbers::sqrt2_v<float>;

//Units
constexpr float Micro = 0.000001f;
constexpr float Milli = 0.001f;
constexpr float Centi = 0.01f;
constexpr float Deci = 0.1f;
constexpr float Kilo = 1000.0f;
constexpr float Mega = 1000000.0f;

const float AU = 1.495978707f * std::powf(10,11);
//Bytes
constexpr float BitToByte = 1 / 8.f;
constexpr float ByteToBit = 8.f;


constexpr int Byte = 1;
constexpr int KiloByte = 1024;
constexpr int MegaByte = 1048576;
constexpr int GigaByte = 1073741824;
constexpr float TeraByte = 1099511627776;

//Bytes

const Vector3f GlobalRight = Vector3f(1.0,0.0,0.0);
const Vector3f GlobalUp = Vector3f(0.0,1.0,0.0);
const Vector3f GlobalFwd = Vector3f(0.0,0.0,1.0);

class RandomEngine
{
public:
	RandomEngine() = delete;
	static std::mt19937_64& engineInstance();

	template <typename T = float>
	static T RandomInRange(const T& a = 0.0f,const T& b = 1.0f);


	template <typename T = float>
	static  T RandomNormal(const T& a = 0.0f,const T& b = 1.0f);

	template <typename T = float>
	static  T RandomBinomial();
};

inline std::mt19937_64& RandomEngine::engineInstance()
{/*
	std::seed_seq seed(
		{
			Timer::GetInstance().GetTotalTime(),
			Timer::GetInstance().GetTotalTime()
		}
	);*/

	static std::mt19937_64 engine = std::mt19937_64( );
	return engine;
}
template<class T>
inline T RandomEngine::RandomInRange(const T& a,const T& b)
{
	std::uniform_real_distribution<T> dist{a,b};
	return dist(engineInstance());
}

template <class T>
inline T RandomEngine::RandomNormal(const T& a,const T& b)
{
	std::normal_distribution<T> dist{a,b};
	return dist(engineInstance());
}
template <class T>
inline T RandomEngine::RandomBinomial()
{
	return (RandomEngine::RandomInRange<T>(0,1) - RandomEngine::RandomInRange<T>(0,1)); //poor mans binomial
}

template <>
inline int RandomEngine::RandomInRange(const int& a,const int& b)
{
	std::uniform_int_distribution dist{a,b};
	return dist(engineInstance());
}

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
	return std::remainderf(a,b);
}


inline float Lerp(float start_value,float end_value,float t)
{
	t = std::clamp<float>(t,0,1); //assures that the given parameter "t" is between 0 and 1

	return start_value + (end_value - start_value) * t;
}
//TODO FIX THIS SHIT; ADD TO CU

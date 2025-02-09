#pragma once
#include <algorithm>
#include <chrono> // For DefaultSeed generation
#include <cmath>
#include <DirectXMath.h>
#include <intrin.h>
#include <numbers>
#include <random>
#include <string>
#include "LinearAlgebra/Vectors.hpp"

#undef min
#undef max
namespace Math {

	constexpr float PI = std::numbers::pi_v<float>;
	constexpr float PI2 = Math::PI * 2.0f;
	constexpr float PIHALF = Math::PI * 0.5f;
	constexpr float PIQUARTER = Math::PI * 0.25f;

	constexpr float DEG_TO_RAD = Math::PI / 180.0f;
	constexpr float  RAD_TO_DEG = 180.0f / Math::PI;
	constexpr float  Sqrt2 = std::numbers::sqrt2_v<float>;
	constexpr double Golden_Ratio = std::numbers::phi;

	// Units
	constexpr float Micro = 0.000001f;
	constexpr float Milli = 0.001f;
	constexpr float Centi = 0.01f;
	constexpr float Deci = 0.1f;
	constexpr float Kilo = 1000.0f;
	constexpr float Mega = 1000000.0f;

	// Bytes
	constexpr float BitToByte = 1 / 8.f;
	constexpr float ByteToBit = 8.f;

	constexpr int  Byte = 1;
	constexpr int  KiloByte = 1024;
	constexpr int  MegaByte = 1048576;
	constexpr int  GigaByte = 1073741824;
	constexpr auto TeraByte = 1099511627776;

	constexpr auto Epsilon = 1e-6;

	// Bytes
	constexpr auto GlobalRight = Vector3f(1.0,0.0,0.0);
	constexpr auto GlobalUp = Vector3f(0.0,1.0,0.0);
	constexpr auto GlobalFwd = Vector3f(0.0,0.0,1.0);

	class Levenshtein {
		static constexpr int deleteScore = 10;
		static constexpr int insertScore = 10;
		static constexpr int substitutionScore = 20;

	public:
		static size_t maxSize(const std::string& aS1,const std::string& aS2);;

		static size_t distance(const std::string& aS1,const std::string& aS2);;
	};

	class RandomEngine {
	public:
		RandomEngine() = delete;
		static std::mt19937_64& engineInstance();

		template <typename T = float>
		static T randomInRange(const T& a = 0.0f,const T& b = 1.0f);

		template <typename T = float>
		static T randomNormal(const T& a = 0.0f,const T& b = 1.0f);

		template <typename T = float>
		static T randomBinomial();
		template <typename T = float>
		static T randomNumberOfType();
	};

	template <typename T>
	bool IsApproximate(const T& aLhv,const T& aRhv) {
		return std::abs(aLhv - aRhv) < std::numeric_limits<T>::epsilon();
	}

	inline size_t Levenshtein::maxSize(const std::string& aS1,const std::string& aS2) {
		return aS1.length() + aS2.length();
	}

	inline size_t Levenshtein::distance(const std::string& aS1,const std::string& aS2) {
		const size_t                     len1 = aS1.size();
		const size_t                     len2 = aS2.size();
		std::vector<std::vector<size_t>> d(len1 + 1,std::vector<size_t>(len2 + 1));

		d[0][0] = 0;
		for(size_t i = 1; i <= len1; ++i) {
			d[i][0] = i * deleteScore;
		}
		for(size_t i = 1; i <= len2; ++i) {
			d[0][i] = i * insertScore;
		}

		for(size_t i = 1; i <= len1; ++i) {
			for(size_t j = 1; j <= len2; ++j) {
				d[i][j] = std::min({
					d[i - 1][j] + deleteScore, d[i][j - 1] + insertScore,
					d[i - 1][j - 1] + (aS1[i - 1] == aS2[j - 1] ? 0 : substitutionScore)
					});
			}
		}

		return d[len1][len2];
	}

	inline std::mt19937_64& RandomEngine::engineInstance() {
		std::random_device rd;
		std::seed_seq      seed({rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()});

		static auto engine = std::mt19937_64(seed);
		return engine;
	}

	template <class T>
	T RandomEngine::randomInRange(const T& a,const T& b) {
		std::uniform_real_distribution<T> dist{a, b};
		return dist(engineInstance());
	}

	//no support for chars or lower than 16 bit sorry 
	template <class T>
	T RandomEngine::randomNumberOfType() {
		if constexpr(std::is_integral_v<T>) {
			static std::uniform_int_distribution<T> staticUniformDistribution;
			return T(staticUniformDistribution(engineInstance()));
		} else if constexpr(std::is_floating_point_v<T>) {
			static std::uniform_real_distribution<T> staticUniformDistribution;
			return T(staticUniformDistribution(engineInstance()));
		} else {
			static_assert(false,"RandomNumberOfType only supports integral and floating point types");
		}
	}

	template <class T>
	T RandomEngine::randomNormal(const T& a,const T& b) {
		std::normal_distribution<T> dist{a, b};
		return dist(engineInstance());
	}

	template <class T>
	T RandomEngine::randomBinomial() {
		return (RandomEngine::randomInRange<T>(0,1) - RandomEngine::randomInRange<T>(0,1)); // poor mans binomial
	}

	template <>
	inline int RandomEngine::randomInRange(const int& a,const int& b) {
		std::uniform_int_distribution dist{a, b};
		return dist(engineInstance());
	}

	inline float Log2(float x) {
		constexpr float invLog2 = 1.442695040888963387004650940071f;
		return std::log(x) * invLog2;
	}

	template <typename T>
	T Mod(T a,T b) {
		T result = a - (a / b) * b;
		return static_cast<T>((result < 0) ? result + b : result);
	}

	template <>
	inline float Mod(float a,float b) {
		return std::remainderf(a,b);
	}

	inline float Lerp(float start_value,float end_value,float t) {
		t = std::clamp<float>(t,0,1); // assures that the given parameter "t" is between 0 and 1

		return start_value + (end_value - start_value) * t;
	}

	// TODO FIX THIS SHIT; ADD TO CU

	inline void threeaxisrot(const float r11,const float r12,const float r21,const float r31,const float r32,
		Vector3f& res) {
		res[0] = atan2(r31,r32);
		res[1] = asin(r21);
		res[2] = atan2(r11,r12);
	}

	inline void quaternion2Euler(const Vector4f& q,Vector3f& res) {
		threeaxisrot(-2 * (q.y * q.z - q.w * q.x),q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
			2 * (q.x * q.z + q.w * q.y),-2 * (q.x * q.y - q.w * q.z),
			q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z,res);
	}

	//
	// Copyright (c) Microsoft. All rights reserved.
	// This code is licensed under the MIT License (MIT).
	// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
	// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
	// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
	// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
	//
	// Developed by Minigraph
	//
	// Author:  James Stanard
	//

	template <typename T>
	__forceinline T AlignUpWithMask(T aValue,const size_t aMask) {
		return reinterpret_cast<T>((reinterpret_cast<size_t>(aValue) + aMask) & ~aMask);
	}

	template <typename T>
	__forceinline T AlignDownWithMask(T aValue,const size_t aMask) {
		return reinterpret_cast<T>(reinterpret_cast<size_t>(aValue) & ~aMask);
	}

	template <typename T>
	__forceinline T AlignUp(T aValue,const size_t aAlignment) {
		return AlignUpWithMask(aValue,aAlignment - 1);
	}

	template <typename T>
	__forceinline T AlignDown(T aValue,const size_t aAlignment) {
		return AlignDownWithMask(aValue,aAlignment - 1);
	}

	template <typename T>
	__forceinline bool IsAligned(T aValue,const size_t aAlignment) {
		return 0 == (static_cast<size_t>(aValue) & (aAlignment - 1));
	}

	template <typename T>
	__forceinline T DivideByMultiple(T aValue,size_t aAlignment) {
		return static_cast<T>((aValue + aAlignment - 1) / aAlignment);
	}

	template <typename T>
	__forceinline bool IsPowerOfTwo(T aValue) {
		return 0 == (aValue & (aValue - 1));
	}

	template <typename T>
	__forceinline bool IsDivisible(T aValue,T aDivisor) {
		return (aValue / aDivisor) * aDivisor == aValue;
	}

	__forceinline uint8_t Log2(uint64_t value) {
		unsigned long mssb; // most significant set bit
		unsigned long lssb; // least significant set bit

		// If perfect power of two (only one set bit), return index of bit.  Otherwise round up
		// fractional log by adding 1 to most signicant set bit's index.
		if(_BitScanReverse64(&mssb,value) > 0 && _BitScanForward64(&lssb,value) > 0) {
			return static_cast<uint8_t>(mssb + (mssb == lssb ? 0 : 1));
		}
		return 0;
	}

	template <typename T>
	__forceinline T AlignPowerOfTwo(T value) {
		return value == 0 ? 0 : 1 << Log2(value);
	};

	inline constexpr uint64_t AlignToPowerOfTwo(uint64_t address, uint64_t align)
	{
		if ((0 == align) || (align & (align - 1))) return address;

		return ((address + (align - 1)) & ~(align - 1));
	}

	inline constexpr uint64_t Align(uint64_t address, uint64_t align)
	{
		if (align == 0 || align == 1) return address;
		uint64_t r = address % align;
		return r ? address + (align - r) : address;
	}

	enum class EZeroTag {
		kZero,
		kOrigin
	};

	enum class EIdentityTag {
		kOne,
		kIdentity
	};

	enum class EXUnitVector {
		kXUnitVector
	};

	enum class EYUnitVector {
		kYUnitVector
	};

	enum class EZUnitVector {
		kZUnitVector
	};

	enum class EWUnitVector {
		kWUnitVector
	};

}
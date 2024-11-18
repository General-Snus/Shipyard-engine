#pragma once 
#include "LinearAlgebra/Vectors.hpp"
#include <DirectXMath.h>
#include <algorithm>
#include <chrono> // For DefaultSeed generation
#include <cmath>
#include <intrin.h>
#include <numbers>
#include <random>
#include <string>

#undef min
#undef max

constexpr float PI = std::numbers::pi_v<float>;
constexpr float PI2 = PI * 2.0f;
constexpr float PIHALF = PI * 0.5f;
constexpr float PIQUARTER = PI * 0.25f;

constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;
constexpr float Sqrt2 = std::numbers::sqrt2_v<float>;
constexpr double Golden_Ratio = 1.618033988749894;

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

constexpr int Byte = 1;
constexpr int KiloByte = 1024;
constexpr int MegaByte = 1048576;
constexpr int GigaByte = 1073741824;
constexpr auto TeraByte = 1099511627776;

// Bytes
constexpr Vector3f GlobalRight = Vector3f(1.0, 0.0, 0.0);
constexpr Vector3f GlobalUp = Vector3f(0.0, 1.0, 0.0);
constexpr Vector3f GlobalFwd = Vector3f(0.0, 0.0, 1.0);

class Levenstein
{
  private:
    const static inline int deletScore = 10;
    const static inline int insertScore = 10;
    const static inline int substitutionScore = 20;

  public:
    inline static size_t MaxSize(const std::string &s1, const std::string &s2)
    {
        return s1.length() + s2.length();
    };
    inline static size_t Distance(const std::string &s1, const std::string &s2)
    {

        const size_t len1 = s1.size();
        const size_t len2 = s2.size();
        std::vector<std::vector<size_t>> d(len1 + 1, std::vector<size_t>(len2 + 1));

        d[0][0] = 0;
        for (size_t i = 1; i <= len1; ++i)
            d[i][0] = i * deletScore;
        for (size_t i = 1; i <= len2; ++i)
            d[0][i] = i * insertScore;

        for (size_t i = 1; i <= len1; ++i)
        {
            for (size_t j = 1; j <= len2; ++j)
            {
                d[i][j] = std::min({d[i - 1][j] + deletScore, d[i][j - 1] + insertScore,
                                    d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : substitutionScore)});
            }
        }

        return d[len1][len2];
    };
};

class RandomEngine
{
  public:
    RandomEngine() = delete;
    static std::mt19937_64 &engineInstance();

    template <typename T = float> static T RandomInRange(const T &a = 0.0f, const T &b = 1.0f);

    template <typename T = float> static T RandomNormal(const T &a = 0.0f, const T &b = 1.0f);

    template <typename T = float> static T RandomBinomial();
	template <typename T = float> static T RandomNumberOfType();
};

template <typename T> inline bool IsApproximate(const T &lhv, const T &rhv)
{
    return std::abs(lhv - rhv) < std::numeric_limits<T>::epsilon();
}

inline std::mt19937_64 &RandomEngine::engineInstance()
{
    std::random_device rd;
    std::seed_seq seed({rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()});

    static auto engine = std::mt19937_64(seed);
    return engine;
}
template <class T> inline T RandomEngine::RandomInRange(const T &a, const T &b)
{
    std::uniform_real_distribution<T> dist{a, b};
    return dist(engineInstance());
}

//no support for chars or lower than 16 bit sorry 
template <class T> inline T RandomEngine::RandomNumberOfType()
{ 
    if constexpr (std::is_integral_v<T>)
	{
		static std::uniform_int_distribution<T> staticUniformDistribution;

		return T(staticUniformDistribution(engineInstance()));
	}
	else if constexpr (std::is_floating_point_v<T>)
	{
		static std::uniform_real_distribution<T> staticUniformDistribution;

		return T(staticUniformDistribution(engineInstance()));
	}
	else
	{
		static_assert(false, "RandomNumberOfType only supports integral and floating point types");
	} 
}

template <class T> inline T RandomEngine::RandomNormal(const T &a, const T &b)
{
    std::normal_distribution<T> dist{a, b};
    return dist(engineInstance());
}
template <class T> inline T RandomEngine::RandomBinomial()
{
    return (RandomEngine::RandomInRange<T>(0, 1) - RandomEngine::RandomInRange<T>(0, 1)); // poor mans binomial
}

template <> inline int RandomEngine::RandomInRange(const int &a, const int &b)
{
    std::uniform_int_distribution dist{a, b};
    return dist(engineInstance());
}

inline float Log2(float x)
{
    const float invLog2 = 1.442695040888963387004650940071f;
    return std::log(x) * invLog2;
}

template <typename T> inline T Mod(T a, T b)
{
    T result = a - (a / b) * b;
    return (T)((result < 0) ? result + b : result);
}

template <> inline float Mod(float a, float b)
{
    return std::remainderf(a, b);
}

inline float Lerp(float start_value, float end_value, float t)
{
    t = std::clamp<float>(t, 0, 1); // assures that the given parameter "t" is between 0 and 1

    return start_value + (end_value - start_value) * t;
}
// TODO FIX THIS SHIT; ADD TO CU

inline void threeaxisrot(const float r11, const float r12, const float r21, const float r31, const float r32,
                         Vector3f &res)
{
    res[0] = atan2(r31, r32);
    res[1] = asin(r21);
    res[2] = atan2(r11, r12);
}

inline void quaternion2Euler(const Vector4f &q, Vector3f &res)
{
    threeaxisrot(-2 * (q.y * q.z - q.w * q.x), q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z,
                 2 * (q.x * q.z + q.w * q.y), -2 * (q.x * q.y - q.w * q.z),
                 q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z, res);
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

template <typename T> __forceinline T AlignUpWithMask(T value, size_t mask)
{
    return (T)(((size_t)value + mask) & ~mask);
}

template <typename T> __forceinline T AlignDownWithMask(T value, size_t mask)
{
    return (T)((size_t)value & ~mask);
}

template <typename T> __forceinline T AlignUp(T value, size_t alignment)
{
    return AlignUpWithMask(value, alignment - 1);
}

template <typename T> __forceinline T AlignDown(T value, size_t alignment)
{
    return AlignDownWithMask(value, alignment - 1);
}

template <typename T> __forceinline bool IsAligned(T value, size_t alignment)
{
    return 0 == ((size_t)value & (alignment - 1));
}

template <typename T> __forceinline T DivideByMultiple(T value, size_t alignment)
{
    return (T)((value + alignment - 1) / alignment);
}

template <typename T> __forceinline bool IsPowerOfTwo(T value)
{
    return 0 == (value & (value - 1));
}

template <typename T> __forceinline bool IsDivisible(T value, T divisor)
{
    return (value / divisor) * divisor == value;
}

__forceinline uint8_t Log2(uint64_t value)
{
    unsigned long mssb; // most significant set bit
    unsigned long lssb; // least significant set bit

    // If perfect power of two (only one set bit), return index of bit.  Otherwise round up
    // fractional log by adding 1 to most signicant set bit's index.
    if (_BitScanReverse64(&mssb, value) > 0 && _BitScanForward64(&lssb, value) > 0)
        return uint8_t(mssb + (mssb == lssb ? 0 : 1));
    else
        return 0;
}

template <typename T> __forceinline T AlignPowerOfTwo(T value)
{
    return value == 0 ? 0 : 1 << Log2(value);
}

using namespace DirectX;

__forceinline XMVECTOR SplatZero()
{
    return XMVectorZero();
}

#if !defined(_XM_NO_INTRINSICS_) && defined(_XM_SSE_INTRINSICS_)

__forceinline XMVECTOR SplatOne(XMVECTOR zero = SplatZero())
{
    __m128i AllBits = _mm_castps_si128(_mm_cmpeq_ps(zero, zero));
    return _mm_castsi128_ps(_mm_slli_epi32(_mm_srli_epi32(AllBits, 25), 23)); // return 0x3F800000
    // return _mm_cvtepi32_ps(_mm_srli_epi32(SetAllBits(zero), 31));				// return (float)1;  (alternate
    // method)
}

#if defined(_XM_SSE4_INTRINSICS_)
INLINE XMVECTOR CreateXUnitVector(XMVECTOR one = SplatOne())
{
    return _mm_insert_ps(one, one, 0x0E);
}
INLINE XMVECTOR CreateYUnitVector(XMVECTOR one = SplatOne())
{
    return _mm_insert_ps(one, one, 0x0D);
}
INLINE XMVECTOR CreateZUnitVector(XMVECTOR one = SplatOne())
{
    return _mm_insert_ps(one, one, 0x0B);
}
INLINE XMVECTOR CreateWUnitVector(XMVECTOR one = SplatOne())
{
    return _mm_insert_ps(one, one, 0x07);
}
INLINE XMVECTOR SetWToZero(FXMVECTOR vec)
{
    return _mm_insert_ps(vec, vec, 0x08);
}
INLINE XMVECTOR SetWToOne(FXMVECTOR vec)
{
    return _mm_blend_ps(vec, SplatOne(), 0x8);
}
#else
__forceinline XMVECTOR CreateXUnitVector(XMVECTOR one = SplatOne())
{
    return _mm_castsi128_ps(_mm_srli_si128(_mm_castps_si128(one), 12));
}
__forceinline XMVECTOR CreateYUnitVector(XMVECTOR one = SplatOne())
{
    XMVECTOR unitx = CreateXUnitVector(one);
    return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(unitx), 4));
}
__forceinline XMVECTOR CreateZUnitVector(XMVECTOR one = SplatOne())
{
    XMVECTOR unitx = CreateXUnitVector(one);
    return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(unitx), 8));
}
__forceinline XMVECTOR CreateWUnitVector(XMVECTOR one = SplatOne())
{
    return _mm_castsi128_ps(_mm_slli_si128(_mm_castps_si128(one), 12));
}
__forceinline XMVECTOR SetWToZero(FXMVECTOR vec)
{
    __m128i MaskOffW = _mm_srli_si128(_mm_castps_si128(_mm_cmpeq_ps(vec, vec)), 4);
    return _mm_and_ps(vec, _mm_castsi128_ps(MaskOffW));
}
__forceinline XMVECTOR SetWToOne(FXMVECTOR vec)
{
    return _mm_movelh_ps(vec, _mm_unpackhi_ps(vec, SplatOne()));
}
#endif

#else // !_XM_SSE_INTRINSICS_

INLINE XMVECTOR SplatOne()
{
    return XMVectorSplatOne();
}
INLINE XMVECTOR CreateXUnitVector()
{
    return g_XMIdentityR0;
}
INLINE XMVECTOR CreateYUnitVector()
{
    return g_XMIdentityR1;
}
INLINE XMVECTOR CreateZUnitVector()
{
    return g_XMIdentityR2;
}
INLINE XMVECTOR CreateWUnitVector()
{
    return g_XMIdentityR3;
}
INLINE XMVECTOR SetWToZero(FXMVECTOR vec)
{
    return XMVectorAndInt(vec, g_XMMask3);
}
INLINE XMVECTOR SetWToOne(FXMVECTOR vec)
{
    return XMVectorSelect(g_XMIdentityR3, vec, g_XMMask3);
}

#endif

enum class EZeroTag
{
    kZero,
    kOrigin
};
enum class EIdentityTag
{
    kOne,
    kIdentity
};
enum class EXUnitVector
{
    kXUnitVector
};
enum class EYUnitVector
{
    kYUnitVector
};
enum class EZUnitVector
{
    kZUnitVector
};
enum class EWUnitVector
{
    kWUnitVector
};

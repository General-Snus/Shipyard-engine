#pragma once
#include <string>
#include <Tools/Utilities/Math.hpp>
#include <exception>
#include <Tools/Utilities/Error.hpp>
#include "d3dx12.h"
#include "Tools/Logging/Logging.h"

namespace Helpers
{
	void ThrowIfFailed(HRESULT hr) noexcept(false);

	inline DXGI_FORMAT GetUAVCompatableFormat(DXGI_FORMAT format)
	{
		DXGI_FORMAT uavFormat = format;

		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_TYPELESS:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			uavFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
			uavFormat = DXGI_FORMAT_R32_FLOAT;
			break;
		}

		return uavFormat;
	}

	inline DXGI_FORMAT GetBaseFormat(DXGI_FORMAT defaultFormat)
	{
		switch (defaultFormat)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_TYPELESS;

		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_TYPELESS;

		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_TYPELESS;

		// 32-bit Z w/ Stencil
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_R32G8X24_TYPELESS;

		// No Stencil
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_TYPELESS;

		// 24-bit Z
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_R24G8_TYPELESS;

		// 16-bit Z w/o Stencil
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
			return DXGI_FORMAT_R16_TYPELESS;

		default:
			return defaultFormat;
		}
	};

	inline DXGI_FORMAT GetDepthFormat(DXGI_FORMAT defaultFormat)
	{
		switch (defaultFormat)
		{
		// 32-bit Z w/ Stencil
		case DXGI_FORMAT_R32G8X24_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
		case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
			return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

		// No Stencil
		case DXGI_FORMAT_R32_TYPELESS:
		case DXGI_FORMAT_D32_FLOAT:
		case DXGI_FORMAT_R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;

		// 24-bit Z
		case DXGI_FORMAT_R24G8_TYPELESS:
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
		case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
		case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

		// 16-bit Z w/o Stencil
		case DXGI_FORMAT_R16_TYPELESS:
		case DXGI_FORMAT_D16_UNORM:
		case DXGI_FORMAT_R16_UNORM:
			return DXGI_FORMAT_R16_UNORM;

		default:
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	// stackoverflow Chuck Walbourn <3
	namespace DX
	{
		// Helper class for COM exceptions
		class com_exception : public std::exception
		{
		public:
			com_exception(HRESULT hr) : result(hr)
			{
			}

			const char* what() const override
			{
				static char s_str[64] = {};
				sprintf_s(s_str, "Failure with HRESULT of %08X", static_cast<unsigned int>(result));
				return s_str;
			}

		private:
			HRESULT result;
		};
	} // namespace DX

	template <typename T, typename U>
	inline static T string_cast(const U& aSomeString)
	{
		aSomeString;
		throw NotImplemented();
	}

	template <>
	inline std::wstring string_cast<std::wstring>(const std::string& aSomeString)
	{
		const auto   sLength = static_cast<int>(aSomeString.length());
		const int    len = MultiByteToWideChar(CP_ACP, 0, aSomeString.c_str(), sLength, nullptr, 0);
		std::wstring result(len, L'\0');
		MultiByteToWideChar(CP_ACP, 0, aSomeString.c_str(), sLength, &result[0], len);
		return result;
	}

	template <>
	inline std::string string_cast<std::string>(const std::wstring& someString)
	{
		const auto  sLength = static_cast<int>(someString.length());
		const int   len = WideCharToMultiByte(CP_ACP, 0, someString.c_str(), sLength, nullptr, 0, nullptr, nullptr);
		std::string result(len, L'\0');
		WideCharToMultiByte(CP_ACP, 0, someString.c_str(), sLength, &result[0], len, nullptr, nullptr);
		return result;
	}

	inline void ThrowIfSucceded(HRESULT hr) noexcept(false)
	{
		if (!FAILED(hr))
		{
			auto err = DX::com_exception(hr);
			LOGGER.Err(err.what());
			throw err;
		}
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

	// This requires SSE4.2 which is present on Intel Nehalem (Nov. 2008)
	// and AMD Bulldozer (Oct. 2011) processors.  I could put a runtime
	// check for this, but I'm just going to assume people playing with
	// DirectX 12 on Windows 10 have fairly recent machines.
#ifdef _M_X64
#define ENABLE_SSE_CRC32 1
#else
#define ENABLE_SSE_CRC32 0
#endif

#if ENABLE_SSE_CRC32
#pragma intrinsic(_mm_crc32_u32)
#pragma intrinsic(_mm_crc32_u64)
#endif
	inline size_t HashRange(const uint32_t* const Begin, const uint32_t* const End, size_t Hash)
	{
#if ENABLE_SSE_CRC32
		auto       Iter64 = reinterpret_cast<const uint64_t*>(::Math::AlignUp(Begin, 8));
		const auto End64 = reinterpret_cast<const uint64_t*const>(::Math::AlignDown(End, 8));

		// If not 64-bit aligned, start with a single u32
		if ((uint32_t*)Iter64 > Begin)
		{
			Hash = _mm_crc32_u32(static_cast<uint32_t>(Hash), *Begin);
		}

		// Iterate over consecutive u64 values
		while (Iter64 < End64)
		{
			Hash = _mm_crc32_u64(Hash, *Iter64++);
		}

		// If there is a 32-bit remainder, accumulate that
		if ((uint32_t*)Iter64 < End)
		{
			Hash = _mm_crc32_u32(static_cast<uint32_t>(Hash), *(uint32_t*)Iter64);
		}
#else
    // An inexpensive hash for CPUs lacking SSE4.2
    for (const uint32_t *Iter = Begin; Iter < End; ++Iter)
        Hash = 16777619U * Hash ^ *Iter;
#endif

		return Hash;
	}

	template <typename T>
	size_t HashState(const T* StateDesc, size_t Count = 1, size_t Hash = 2166136261U)
	{
		static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4, "State m_object is not word-aligned");
		return HashRange(static_cast<uint32_t*>(StateDesc), static_cast<uint32_t*>(StateDesc + Count), Hash);
	}
} // namespace Helpers

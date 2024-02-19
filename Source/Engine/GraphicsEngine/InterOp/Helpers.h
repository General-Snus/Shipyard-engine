#pragma once
#include <string>
#include <Tools/Utilities/Math.hpp>

#include <DirectX/XTK/DirectXHelpers.h>
#include <exception>
#include "Tools/Logging/Logging.h"

namespace Helpers
{
	//stackoverflow Chuck Walbourn <3
	namespace DX
	{
		// Helper class for COM exceptions
		class com_exception : public std::exception
		{
		public:
			com_exception(HRESULT hr) : result(hr) {}

			virtual const char* what() const override
			{
				static char s_str[64] = {};
				sprintf_s(s_str,"Failure with HRESULT of %08X",
					static_cast<unsigned int>(result));
				return s_str;
			}

		private:
			HRESULT result;
		};
	}


	template<typename T,typename U>
	inline static T string_cast(const U& someString)
	{
		someString;
		return nullptr;
	}

	template<>
	inline std::wstring string_cast<std::wstring>(const std::string& someString)
	{
		const int sLength = static_cast<int>(someString.length());
		const int len = MultiByteToWideChar(CP_ACP,0,someString.c_str(),sLength,nullptr,0);
		std::wstring result(len,L'\0');
		MultiByteToWideChar(CP_ACP,0,someString.c_str(),sLength,&result[0],len);
		return result;
	}

	template<>
	inline std::string string_cast<std::string>(const std::wstring& someString)
	{
		const int sLength = static_cast<int>(someString.length());
		const int len = WideCharToMultiByte(CP_ACP,0,someString.c_str(),sLength,nullptr,0,nullptr,nullptr);
		std::string result(len,L'\0');
		WideCharToMultiByte(CP_ACP,0,someString.c_str(),sLength,&result[0],len,nullptr,nullptr);
		return result;
	}

	inline void ThrowIfFailed(HRESULT hr) noexcept(false)
	{
		if (FAILED(hr))
		{
			auto err = DX::com_exception(hr);
			Logger::Err(err.what());
			throw err;
		}
	}
	inline void ThrowIfSucceded(HRESULT hr) noexcept(false)
	{
		if (!FAILED(hr))
		{
			auto err = DX::com_exception(hr);
			Logger::Err(err.what());
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
	inline size_t HashRange(const uint32_t* const Begin,const uint32_t* const End,size_t Hash)
	{
#if ENABLE_SSE_CRC32
		const uint64_t* Iter64 = (const uint64_t*)::AlignUp(Begin,8);
		const uint64_t* const End64 = (const uint64_t* const)::AlignDown(End,8);

		// If not 64-bit aligned, start with a single u32
		if ((uint32_t*)Iter64 > Begin)
			Hash = _mm_crc32_u32((uint32_t)Hash,*Begin);

		// Iterate over consecutive u64 values
		while (Iter64 < End64)
			Hash = _mm_crc32_u64((uint64_t)Hash,*Iter64++);

		// If there is a 32-bit remainder, accumulate that
		if ((uint32_t*)Iter64 < End)
			Hash = _mm_crc32_u32((uint32_t)Hash,*(uint32_t*)Iter64);
#else
		// An inexpensive hash for CPUs lacking SSE4.2
		for (const uint32_t* Iter = Begin; Iter < End; ++Iter)
			Hash = 16777619U * Hash ^ *Iter;
#endif

		return Hash;
	}

	template <typename T> inline size_t HashState(const T* StateDesc,size_t Count = 1,size_t Hash = 2166136261U)
	{
		static_assert((sizeof(T) & 3) == 0 && alignof(T) >= 4,"State object is not word-aligned");
		return HashRange((uint32_t*)StateDesc,(uint32_t*)(StateDesc + Count),Hash);
	}

}

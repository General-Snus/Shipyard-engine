#pragma once
#include <string>

namespace Helpers
{
	template<typename T, typename U>
	inline static T string_cast(const U& someString)
	{
		someString;
		return nullptr;
	}

	template<>
	inline std::wstring string_cast<std::wstring>(const std::string& someString)
	{
		const int sLength = static_cast<int>(someString.length());
		const int len = MultiByteToWideChar(CP_ACP, 0, someString.c_str(), sLength, 0, 0);
		std::wstring result(len, L'\0');
		MultiByteToWideChar(CP_ACP, 0, someString.c_str(), sLength, &result[0], len);
		return result;
	} 

	template<>
	inline std::string string_cast<std::string>(const std::wstring& someString)
	{
		const int sLength = static_cast<int>(someString.length());
		const int len = WideCharToMultiByte(CP_ACP, 0, someString.c_str(), sLength, 0, 0, 0, 0);
		std::string result(len, L'\0');
		WideCharToMultiByte(CP_ACP, 0, someString.c_str(), sLength, &result[0], len, 0, 0);
		return result;
	}
}

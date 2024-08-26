#pragma once
#include <string>

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif


// Enable heterogeneous lookup for maps with string keys.
struct string_hash
{
	using is_transparent = void;
	[[nodiscard]] size_t operator()(const char* aPtr) const
	{
		return std::hash<std::string_view>{}(aPtr);
	}

	[[nodiscard]] size_t operator()(std::string_view aStringView) const
	{
		return std::hash<std::string_view>{}(aStringView);
	}

	[[nodiscard]] size_t operator()(const std::string& aString) const
	{
		return std::hash<std::string>{}(aString);
	}
};


/**
 * @brief Left-Trim the specified string in-place for occurrences of whitespace characters (space, tab, newline etc).
 * @param s The string to trim.
*/
inline void ltrim_in_place(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

/**
 * @brief Right-Trim the specified string in-place for occurrences of whitespace characters (space, tab, newline etc).
 * @param s The string to trim.
*/
inline void rtrim_in_place(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

/**
 * @brief Trim the specified string in-place for occurrences of whitespace characters (space, tab, newline etc).
 * @param s The string to trim.
*/
inline void trim_in_place(std::string& s) {
	ltrim_in_place(s);
	rtrim_in_place(s);
}
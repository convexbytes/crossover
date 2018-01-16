#pragma once

#include <string>
#include <codecvt>
#include <type_traits>

namespace crossover

{
	namespace string
	{
		/// @brief Converts wstring utf16 to string utf8
		/// Useful when working with utility::string_t, which can be 
		/// either a std::wstring or std::string, and we need a std::string regardless.
		/// 
		inline std::string utf16_to_utf8(std::wstring const & s)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8converter{};
			return utf8converter.to_bytes(s);
		}

		/// @brief Returns s
		/// Useful when working with utility::string_t, which can be 
		/// either a std::wstring or std::string, and we need a std::string regardless.
		inline std::string utf16_to_utf8(std::string const & s)
		{
			return s;
		}

		/// @brief Converts std::string utf8 to std::wstring utf16
		inline std::wstring utf8_to_utf16(std::string const & s)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8converter{};
			return utf8converter.from_bytes(s);
		}

		/// @brief Converts std::string utf8 to std::wstring utf16
		/// Useful when working with utility::string_t, which can be 
		/// either a std::wstring or std::string, and we need a utility::string_t regardless.

		inline utility::string_t to_utility_string(std::string const & s)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8converter{};
			return utf8converter.from_bytes(s);
		}

		template<typename T>
		struct is_std_string : std::false_type{};

		template<>
		struct is_std_string <std::string> : std::true_type{};

		template<typename T>
		struct is_std_wstring : std::false_type {};

		template<>
		struct is_std_wstring <std::wstring> : std::true_type {};

		template <typename T>
		utility::string_t to_utility_string(std::string const & s)
		{
			if (is_std_string<utility::string_t>::value)
			{
				return s;
			}
			else
			{
				return utf8_to_utf16(s);
			}
		}

		template <typename T>
		utility::string_t to_utility_string(std::wstring const & s)
		{
			if (is_std_wstring<utility::string_t>::value)
			{
				return s;
			}
			else
			{
				return utf16_to_utf8(s);
			}
		}

	}
}
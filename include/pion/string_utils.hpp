/*
 * Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
 * Copyright 2015, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   string_utils.hpp
 * Author: alex
 *
 * Created on January 2, 2015, 12:59 PM
 */

#ifndef PION_STRING_UTILS_HPP
#define PION_STRING_UTILS_HPP

#include <cstdint>
#include <string>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <pion/config.hpp>

namespace pion {
namespace utils {

/**
 * Allocates a buffer and copies there contents of the string.
 * Allocated buffer must be freed by the caller.
 * 
 * @param str string to copy
 * @return allocated buffer
 */
PION_API char* alloc_copy(const std::string& str) PION_NOEXCEPT;

/**
 * Splits string into vector using specified character as a delimiter,
 * empty result parts are ignored
 * 
 * @param str string to split
 * @param delim delimiter character
 * @return vector containing splitted parts
 */
PION_API std::vector<std::string> split(const std::string& str, char delim);

/**
 * Checks whether one string starts with another one
 * 
 * @param value string to check
 * @param start string start
 * @return true if string starts with specified ending, false otherwise
 */
PION_API bool starts_with(const std::string& value, const std::string& start);

/**
 * Checks whether one string ends with another one
 * 
 * @param value string to check
 * @param ending string ending
 * @return true if string ends with specified ending, false otherwise
 */
PION_API bool ends_with(const std::string& value, const std::string& ending);

/**
 * Returns new string containing specified path but without
 * the filename (last non-ending-with-slash element of the path)
 * 
 * @param file_path file path
 * @return parent directory path
 */
PION_API std::string strip_filename(const std::string& file_path);

/**
 * Returns new string containing specified path but without
 * the parent directory (without everything before the 
 * last non-ending-with-slash element of the path)
 * 
 * @param file_path file path
 * @return filename
 */
PION_API std::string strip_parent_dir(const std::string& file_path);

/**
 * Trims specified string from left and from right using "std::isspace"
 * to check empty bytes, does not support Unicode
 * 
 * @param s string to trim
 * @return trimmed string
 */
PION_API std::string trim(const std::string& s);

/**
 * Case insensitive byte-to-byte string comparison, does not support Unicode
 * 
 * @param str1 first string
 * @param str2 seconds string
 * @return true if strings equal ignoring case, false otherwise
 */
// http://stackoverflow.com/a/27813
PION_API bool iequals(const std::string& str1, const std::string& str2);


/**
* Case sensitive byte-to-byte string comparison, does not support Unicode
*
* @param str1 first string
* @param str2 seconds string
* @return true if strings equal, false otherwise
*/
PION_API bool equals(const std::string& str1, const std::string& str2);

/**
 * Finds and replaces all "snippet" substrings in specified 
 * string with "replacement"
 * 
 * @param str input string
 * @param snippet substring to replace
 * @param replacement replacement string
 * @return input string reference
 */
PION_API std::string& replace_all(std::string& str, const std::string& snippet, const std::string& replacement);

/**
 * Reference to empty string
 * 
 * @return empty string reference
 */
PION_API const std::string& empty_string();

/**
* Unescapes specified URL-encoded string (a%20value+with%20spaces)
*
* @param str URL-encoded string
* @return unescaped (plain) string
*/
PION_API std::string url_decode(const std::string& str);

/**
* Encodes specified string so that it is safe for URLs (with%20spaces)
*
* @param str string to encode
* @return escaped string
*/
PION_API std::string url_encode(const std::string& str);

/**
* convert string to lower
*
* @param str string
* @return lower string
*/
PION_API std::string to_lower(const std::string& str);

/**
* Generic `to_string` implementation
*
* @param obj object to stringify
* @return string representation of specified value
*/
template<typename T>
std::string to_string_any(const T& obj) {
	std::stringstream ss{};
	ss << obj;
	return ss.str();
}

/**
* Generic `to_string` implementation, already exists as `std::to_string`
* in most C++11 compilers except GCC 4.8 on Android
*
* @param obj object to stringify
* @return string representation of specified value
*/
template<typename T>
std::string to_string(const T& obj) {
#ifndef STATICLIB_ANDROID
	return std::to_string(obj);
#else
	return to_string_any(obj);
#endif // STATICLIB_ANDROID
}

/**
* Converts bool value to "true" or "false" string.
*
* @param val boolean value
* @return "true" or "false" string
*/
inline std::string to_string_bool(bool val) {
	return val ? "true" : "false";
}

} // namespace
}

#endif /* PION_STRING_UTILS_HPP */


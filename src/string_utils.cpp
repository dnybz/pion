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
 * File:   string_utils.cpp
 * Author: alex
 *
 * Created on January 13, 2015, 9:03 PM
 */

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <string>
#include <exception>
#include <sstream>
#include "pion/config.hpp"
#include "pion/string_utils.hpp"

namespace pion {
namespace utils {

char* alloc_copy(const std::string& str) PION_NOEXCEPT {
    auto len = str.length();
    char* msg = static_cast<char*> (malloc(len + 1));
    if (nullptr == msg) {
        char* err = static_cast<char*> (malloc(2));
        err[0] = 'E';
        err[1] = '\0';
        return err;
    }
    msg[len] = '\0';
    memcpy(msg, str.c_str(), len);
    return msg;
}

std::vector<std::string> split(const std::string& str, char delim) {
    std::stringstream ss{str};
    std::vector<std::string> res{};
    std::string item{};    
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            res.push_back(item);
        }
    }
    return res;
}

// http://stackoverflow.com/a/8095276/314015
bool starts_with(const std::string& value, const std::string& start) {
    return 0 == value.compare(0, start.length(), start);
}

// http://stackoverflow.com/a/874160/314015
bool ends_with(std::string const& value, std::string const& ending) {
    if (value.length() >= ending.length()) {
        return (0 == value.compare(value.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::string strip_filename(const std::string& file_path) {
    std::string::size_type pos = file_path.find_last_of("/\\");
    if (std::string::npos != pos && pos < file_path.length() - 1) {
        return std::string(file_path.data(), pos + 1);
    }
    return std::string(file_path.data(), file_path.length());
}

std::string strip_parent_dir(const std::string& file_path) {
    std::string unslashed = file_path;
    std::replace(unslashed.begin(), unslashed.end(), '\\', '/');
    while (!unslashed.empty() && '/' == unslashed.at(unslashed.length() - 1)) {
        unslashed.resize(unslashed.length() - 1);
    }
    std::string::size_type pos = unslashed.find_last_of('/');
    if (std::string::npos == pos) {
        return std::string(file_path.data(), file_path.length());
    } 
    return std::string(file_path, pos + 1);
}

// http://stackoverflow.com/a/17976541
std::string trim(const std::string& s) {
    auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) {
        return std::isspace(c);
    });
    return std::string(wsfront, std::find_if_not(s.rbegin(), std::string::const_reverse_iterator(wsfront), [](int c) {
        return std::isspace(c);
    }).base());
}

// http://stackoverflow.com/a/27813
bool iequals(const std::string& str1, const std::string& str2) {
    if (str1.size() != str2.size()) {
        return false;
    }
    for (std::string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2) {
        if (std::tolower(*c1) != std::tolower(*c2)) {
            return false;
        }
    }
    return true;
}

bool equals(const std::string& str1, const std::string& str2)
{
	if (str1.size() != str2.size()) {
		return false;
	}
	for (std::string::const_iterator c1 = str1.begin(), c2 = str2.begin(); c1 != str1.end(); ++c1, ++c2) {
		if (*c1 != *c2) {
			return false;
		}
	}
	return true;
}

std::string& replace_all(std::string& str, const std::string& snippet, const std::string& replacement) {
    if (snippet.empty()) {
        return str;
    }
    auto pos = std::string::npos;
    while (std::string::npos != (pos = str.find(snippet))) {
        str.replace(pos, snippet.length(), replacement);
    }
    return str;
}

const std::string& empty_string() {
    static std::string empty{""};
    return empty;
}

std::string url_decode(const std::string& str) {
	char decode_buf[3];
	std::string result;
	result.reserve(str.size());

	for (std::string::size_type pos = 0; pos < str.size(); ++pos) {
		switch (str[pos]) {
		case '+':
			// convert to space character
			result += ' ';
			break;
		case '%':
			// decode hexadecimal value
			if (pos + 2 < str.size()) {
				decode_buf[0] = str[++pos];
				decode_buf[1] = str[++pos];
				decode_buf[2] = '\0';

				char decoded_char = static_cast<char> (std::strtol(decode_buf, 0, 16));

				// decoded_char will be '\0' if strtol can't parse decode_buf as hex
				// (or if decode_buf == "00", which is also not valid).
				// In this case, recover from error by not decoding.
				if (decoded_char == '\0') {
					result += '%';
					pos -= 2;
				}
				else
					result += decoded_char;
			}
			else {
				// recover from error by not decoding character
				result += '%';
			}
			break;
		default:
			// character does not need to be escaped
			result += str[pos];
		}
	};

	return result;
}

std::string url_encode(const std::string& str) {
	char encode_buf[4];
	std::string result;
	encode_buf[0] = '%';
	result.reserve(str.size());

	// character selection for this algorithm is based on the following url:
	// http://www.blooberry.com/indexdot/html/topics/urlencoding.htm
	for (std::string::size_type pos = 0; pos < str.size(); ++pos) {
		switch (str[pos]) {
		default:
			if (str[pos] > 32 && str[pos] < 127) {
				// character does not need to be escaped
				result += str[pos];
				break;
			}
			// else pass through to next case
			// https://stackoverflow.com/a/45137452/314015
			// fall through
		case ' ':
		case '$': case '&': case '+': case ',': case '/': case ':':
		case ';': case '=': case '?': case '@': case '"': case '<':
		case '>': case '#': case '%': case '{': case '}': case '|':
		case '\\': case '^': case '~': case '[': case ']': case '`':
			// the character needs to be encoded
			std::sprintf(encode_buf + 1, "%.2X", (unsigned char)(str[pos]));
			result += encode_buf;
			break;
		}
	};

	return result;
}

std::string to_lower(const std::string& str)
{
	std::string t = str;
	transform(t.begin(), t.end(), t.begin(), tolower);
	return t;
}

} // namespace
} 


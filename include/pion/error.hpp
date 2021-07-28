// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#ifndef __PION_ERROR_HEADER__
#define __PION_ERROR_HEADER__

#include <pion/config.hpp>

namespace pion {

	/**
	* Base exception class for various exceptions in staticlib modules
	*/
	class exception : public std::exception {
	protected:
		/**
		* Error message
		*/
		std::string message;

	public:
		/**
		* Default constructor
		*/
		exception() = default;

		/**
		* Constructor with message
		*
		* @param msg error message
		*/
		exception(const std::string& message) :
			message(message.data(), message.length()) { }

		/**
		* Returns error message
		*
		* @return error message
		*/
		virtual const char* what() const PION_NOEXCEPT {
			return message.c_str();
		}
	};
}

#endif

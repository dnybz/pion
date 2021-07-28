// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#ifndef __PION_HTTP_TYPES_HEADER__
#define __PION_HTTP_TYPES_HEADER__

#include <string>
#include <pion/config.hpp>
#include <pion/hash_map.hpp>


namespace pion {    // begin namespace pion
namespace http {    // begin namespace http


///
/// types: common data types used by HTTP
/// 
struct PION_API types
{
    /// virtual destructor
    virtual ~types() {}
    
    // generic strings used by HTTP
    static const std::string    STRING_EMPTY;
    static const std::string    STRING_CRLF;
    static const std::string    STRING_HTTP_VERSION;
    static const std::string    HEADER_NAME_VALUE_DELIMITER;
    static const std::string    COOKIE_NAME_VALUE_DELIMITER;
    
    // common HTTP header names
    static const std::string    HEADER_HOST;
    static const std::string    HEADER_COOKIE;
    static const std::string    HEADER_SET_COOKIE;
    static const std::string    HEADER_CONNECTION;
    static const std::string    HEADER_CONTENT_TYPE;
    static const std::string    HEADER_CONTENT_LENGTH;
    static const std::string    HEADER_CONTENT_LOCATION;
    static const std::string    HEADER_CONTENT_ENCODING;
    static const std::string    HEADER_CONTENT_DISPOSITION;
    static const std::string    HEADER_LAST_MODIFIED;
    static const std::string    HEADER_IF_MODIFIED_SINCE;
    static const std::string    HEADER_TRANSFER_ENCODING;
    static const std::string    HEADER_LOCATION;
    static const std::string    HEADER_AUTHORIZATION;
    static const std::string    HEADER_REFERER;
    static const std::string    HEADER_USER_AGENT;
    static const std::string    HEADER_X_FORWARDED_FOR;
    static const std::string    HEADER_X_POWERED_BY;
    static const std::string    HEADER_X_REQUESTED_WITH;
    static const std::string    HEADER_X_UA_COMPATIBLE;
    static const std::string    HEADER_CLIENT_IP;
    static const std::string    HEADER_CACHE_CONTROL;
    static const std::string    HEADER_ORIGIN;
    static const std::string    HEADER_ACCEPT;
    static const std::string    HEADER_ALLOW;
    static const std::string    HEADER_CORS_ALLOW_ORIGIN;
    static const std::string    HEADER_CORS_ALLOW_CREDENTIALS;
    static const std::string    HEADER_CORS_ALLOW_METHODS;
    static const std::string    HEADER_CORS_ALLOW_HEADERS;
    static const std::string    HEADER_CORS_REQUEST_METHOD;

    // common HTTP content types
    static const std::string    CONTENT_TYPE_HTML;
    static const std::string    CONTENT_TYPE_TEXT;
    static const std::string    CONTENT_TYPE_XML;
    static const std::string    CONTENT_TYPE_APP_JSON;
    static const std::string    CONTENT_TYPE_APP_XHTML_XML;
    static const std::string    CONTENT_TYPE_URLENCODED;
    static const std::string    CONTENT_TYPE_MULTIPART_FORM_DATA;
    static const std::string    CONTENT_TYPE_HTML_UTF8;
    static const std::string    CONTENT_TYPE_TEXT_UTF8;
    static const std::string    CONTENT_TYPE_XML_UTF8;
    static const std::string    CONTENT_TYPE_APP_JSON_UTF8;
    static const std::string    CONTENT_TYPE_APP_XHTML_XML_UTF8;
    
    // common HTTP request methods
    static const std::string    REQUEST_METHOD_HEAD;
    static const std::string    REQUEST_METHOD_GET;
    static const std::string    REQUEST_METHOD_PUT;
    static const std::string    REQUEST_METHOD_POST;
    static const std::string    REQUEST_METHOD_DELETE;
    
    // common HTTP response messages
    static const std::string    RESPONSE_MESSAGE_OK;
    static const std::string    RESPONSE_MESSAGE_CREATED;
    static const std::string    RESPONSE_MESSAGE_ACCEPTED;
    static const std::string    RESPONSE_MESSAGE_NO_CONTENT;
    static const std::string    RESPONSE_MESSAGE_FOUND;
    static const std::string    RESPONSE_MESSAGE_UNAUTHORIZED;
    static const std::string    RESPONSE_MESSAGE_FORBIDDEN;
    static const std::string    RESPONSE_MESSAGE_NOT_FOUND;
    static const std::string    RESPONSE_MESSAGE_METHOD_NOT_ALLOWED;
    static const std::string    RESPONSE_MESSAGE_NOT_MODIFIED;
    static const std::string    RESPONSE_MESSAGE_BAD_REQUEST;
    static const std::string    RESPONSE_MESSAGE_CONFLICT;
    static const std::string    RESPONSE_MESSAGE_PRECONDITION_FAILED;
    static const std::string    RESPONSE_MESSAGE_UNSUPPORTED_MEDIA_TYPE;
    static const std::string    RESPONSE_MESSAGE_UNPROCESSABLE_ENTITY;
    static const std::string    RESPONSE_MESSAGE_UPGRADE_REQUIRED;
    static const std::string    RESPONSE_MESSAGE_SERVER_ERROR;
    static const std::string    RESPONSE_MESSAGE_NOT_IMPLEMENTED;
    static const std::string    RESPONSE_MESSAGE_CONTINUE;

    // common HTTP response codes
    static const unsigned int   RESPONSE_CODE_OK;
    static const unsigned int   RESPONSE_CODE_CREATED;
    static const unsigned int   RESPONSE_CODE_ACCEPTED;
    static const unsigned int   RESPONSE_CODE_NO_CONTENT;
    static const unsigned int   RESPONSE_CODE_FOUND;
    static const unsigned int   RESPONSE_CODE_UNAUTHORIZED;
    static const unsigned int   RESPONSE_CODE_FORBIDDEN;
    static const unsigned int   RESPONSE_CODE_NOT_FOUND;
    static const unsigned int   RESPONSE_CODE_METHOD_NOT_ALLOWED;
    static const unsigned int   RESPONSE_CODE_NOT_MODIFIED;
    static const unsigned int   RESPONSE_CODE_BAD_REQUEST;
    static const unsigned int   RESPONSE_CODE_CONFLICT;
    static const unsigned int   RESPONSE_CODE_PRECONDITION_FAILED;
    static const unsigned int   RESPONSE_CODE_UNSUPPORTED_MEDIA_TYPE;
    static const unsigned int   RESPONSE_CODE_UNPROCESSABLE_ENTITY;
    static const unsigned int   RESPONSE_CODE_UPGRADE_REQUIRED;
    static const unsigned int   RESPONSE_CODE_SERVER_ERROR;
    static const unsigned int   RESPONSE_CODE_NOT_IMPLEMENTED;
    static const unsigned int   RESPONSE_CODE_CONTINUE;
    

    /// converts time_t format into an HTTP-date string
    static std::string get_date_string(const time_t t);

    /// builds an HTTP query string from a collection of query parameters
    static std::string make_query_string(const ihash_multimap& query_params);
    
    /**
     * creates a "Set-Cookie" header
     *
     * @param name the name of the cookie
     * @param value the value of the cookie
     * @param path the path of the cookie
     * @param has_max_age true if the max_age value should be set
     * @param max_age the life of the cookie, in seconds (0 = discard)
     *
     * @return the new "Set-Cookie" header
     */
    static std::string make_set_cookie_header(const std::string& name,
                                              const std::string& value,
                                              const std::string& path,
                                              const bool has_max_age = false,
                                              const unsigned long max_age = 0);     
};


}   // end namespace http
}   // end namespace pion

#endif

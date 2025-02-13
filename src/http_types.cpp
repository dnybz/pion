// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#include <pion/http/types.hpp>
#include <pion/algorithm.hpp>
#include <cstdio>
#include <ctime>
#include <mutex>


namespace pion {    // begin namespace pion
namespace http {    // begin namespace http


// generic strings used by HTTP
const std::string   types::STRING_EMPTY;
const std::string   types::STRING_CRLF("\x0D\x0A");
const std::string   types::STRING_HTTP_VERSION("HTTP/");
const std::string   types::HEADER_NAME_VALUE_DELIMITER(": ");
const std::string   types::COOKIE_NAME_VALUE_DELIMITER("=");

// common HTTP header names
const std::string   types::HEADER_HOST("Host");
const std::string   types::HEADER_COOKIE("Cookie");
const std::string   types::HEADER_SET_COOKIE("Set-Cookie");
const std::string   types::HEADER_CONNECTION("Connection");
const std::string   types::HEADER_CONTENT_TYPE("Content-Type");
const std::string   types::HEADER_CONTENT_LENGTH("Content-Length");
const std::string   types::HEADER_CONTENT_LOCATION("Content-Location");
const std::string   types::HEADER_CONTENT_ENCODING("Content-Encoding");
const std::string   types::HEADER_CONTENT_DISPOSITION("Content-Disposition");
const std::string   types::HEADER_LAST_MODIFIED("Last-Modified");
const std::string   types::HEADER_IF_MODIFIED_SINCE("If-Modified-Since");
const std::string   types::HEADER_TRANSFER_ENCODING("Transfer-Encoding");
const std::string   types::HEADER_LOCATION("Location");
const std::string   types::HEADER_AUTHORIZATION("Authorization");
const std::string   types::HEADER_REFERER("Referer");
const std::string   types::HEADER_USER_AGENT("User-Agent");
const std::string   types::HEADER_X_FORWARDED_FOR("X-Forwarded-For");
const std::string   types::HEADER_X_POWERED_BY("X-Powered-By");
const std::string   types::HEADER_X_REQUESTED_WITH("X-Requested-With");
const std::string   types::HEADER_X_UA_COMPATIBLE("X-UA-Compatible");
const std::string   types::HEADER_CLIENT_IP("Client-IP");
const std::string   types::HEADER_CACHE_CONTROL("Cache-Control");
const std::string   types::HEADER_ORIGIN("Origin");
const std::string   types::HEADER_ACCEPT("Accept");
const std::string   types::HEADER_ALLOW("Allow");
const std::string   types::HEADER_CORS_ALLOW_ORIGIN("Access-Control-Allow-Origin");
const std::string   types::HEADER_CORS_ALLOW_CREDENTIALS("Access-Control-Allow-Credentials");
const std::string   types::HEADER_CORS_ALLOW_METHODS("Access-Control-Allow-Methods");
const std::string   types::HEADER_CORS_ALLOW_HEADERS("Access-Control-Allow-Headers");
const std::string   types::HEADER_CORS_REQUEST_METHOD("Access-Control-Request-Method");

// common HTTP content types
const std::string   types::CONTENT_TYPE_HTML("text/html");
const std::string   types::CONTENT_TYPE_TEXT("text/plain");
const std::string   types::CONTENT_TYPE_XML("text/xml");
const std::string   types::CONTENT_TYPE_APP_JSON("application/json");
const std::string   types::CONTENT_TYPE_APP_XHTML_XML("application/xhtml+xml");
const std::string   types::CONTENT_TYPE_URLENCODED("application/x-www-form-urlencoded");
const std::string   types::CONTENT_TYPE_MULTIPART_FORM_DATA("multipart/form-data");
const std::string   types::CONTENT_TYPE_HTML_UTF8("text/html" "; charset=utf-8");
const std::string   types::CONTENT_TYPE_TEXT_UTF8("text/plain" "; charset=utf-8");
const std::string   types::CONTENT_TYPE_XML_UTF8("text/xml" "; charset=utf-8");
const std::string   types::CONTENT_TYPE_APP_JSON_UTF8("application/json" "; charset=utf-8");
const std::string   types::CONTENT_TYPE_APP_XHTML_XML_UTF8("application/xhtml+xml" "; charset=utf-8");

// common HTTP request methods
const std::string   types::REQUEST_METHOD_HEAD("HEAD");
const std::string   types::REQUEST_METHOD_GET("GET");
const std::string   types::REQUEST_METHOD_PUT("PUT");
const std::string   types::REQUEST_METHOD_POST("POST");
const std::string   types::REQUEST_METHOD_DELETE("DELETE");

// common HTTP response messages
const std::string   types::RESPONSE_MESSAGE_OK("OK");
const std::string   types::RESPONSE_MESSAGE_CREATED("Created");
const std::string   types::RESPONSE_MESSAGE_ACCEPTED("Accepted");
const std::string   types::RESPONSE_MESSAGE_NO_CONTENT("No Content");
const std::string   types::RESPONSE_MESSAGE_FOUND("Found");
const std::string   types::RESPONSE_MESSAGE_UNAUTHORIZED("Unauthorized");
const std::string   types::RESPONSE_MESSAGE_FORBIDDEN("Forbidden");
const std::string   types::RESPONSE_MESSAGE_NOT_FOUND("Not Found");
const std::string   types::RESPONSE_MESSAGE_METHOD_NOT_ALLOWED("Method Not Allowed");
const std::string   types::RESPONSE_MESSAGE_NOT_MODIFIED("Not Modified");
const std::string   types::RESPONSE_MESSAGE_BAD_REQUEST("Bad Request");
const std::string   types::RESPONSE_MESSAGE_CONFLICT("Conflict");
const std::string   types::RESPONSE_MESSAGE_PRECONDITION_FAILED("Precondition Failed");
const std::string   types::RESPONSE_MESSAGE_UNSUPPORTED_MEDIA_TYPE("Unsupported Media Type");
const std::string   types::RESPONSE_MESSAGE_UNPROCESSABLE_ENTITY("Unprocessable Entity");
const std::string   types::RESPONSE_MESSAGE_UPGRADE_REQUIRED("Upgrade Required");
const std::string   types::RESPONSE_MESSAGE_SERVER_ERROR("Server Error");
const std::string   types::RESPONSE_MESSAGE_NOT_IMPLEMENTED("Not Implemented");
const std::string   types::RESPONSE_MESSAGE_CONTINUE("Continue");

// common HTTP response codes
const unsigned int  types::RESPONSE_CODE_OK = 200;
const unsigned int  types::RESPONSE_CODE_CREATED = 201;
const unsigned int  types::RESPONSE_CODE_ACCEPTED = 202;
const unsigned int  types::RESPONSE_CODE_NO_CONTENT = 204;
const unsigned int  types::RESPONSE_CODE_FOUND = 302;
const unsigned int  types::RESPONSE_CODE_UNAUTHORIZED = 401;
const unsigned int  types::RESPONSE_CODE_FORBIDDEN = 403;
const unsigned int  types::RESPONSE_CODE_NOT_FOUND = 404;
const unsigned int  types::RESPONSE_CODE_METHOD_NOT_ALLOWED = 405;
const unsigned int  types::RESPONSE_CODE_NOT_MODIFIED = 304;
const unsigned int  types::RESPONSE_CODE_BAD_REQUEST = 400;
const unsigned int  types::RESPONSE_CODE_CONFLICT = 409;
const unsigned int  types::RESPONSE_CODE_PRECONDITION_FAILED = 412;
const unsigned int  types::RESPONSE_CODE_UNSUPPORTED_MEDIA_TYPE = 415;
const unsigned int  types::RESPONSE_CODE_UNPROCESSABLE_ENTITY = 422;
const unsigned int  types::RESPONSE_CODE_UPGRADE_REQUIRED = 426;
const unsigned int  types::RESPONSE_CODE_SERVER_ERROR = 500;
const unsigned int  types::RESPONSE_CODE_NOT_IMPLEMENTED = 501;
const unsigned int  types::RESPONSE_CODE_CONTINUE = 100;


// static member functions

std::string types::get_date_string(const time_t t)
{
    // use mutex since time functions are normally not thread-safe
    static std::mutex time_mutex;
    static const char *TIME_FORMAT = "%a, %d %b %Y %H:%M:%S GMT";
    static const unsigned int TIME_BUF_SIZE = 100;
    char time_buf[TIME_BUF_SIZE+1];

    std::unique_lock<std::mutex> time_lock(time_mutex);
    if (strftime(time_buf, TIME_BUF_SIZE, TIME_FORMAT, gmtime(&t)) == 0)
        time_buf[0] = '\0'; // failed; resulting buffer is indeterminate
    time_lock.unlock();

    return std::string(time_buf);
}

std::string types::make_query_string(const ihash_multimap& query_params)
{
    std::string query_string;
    for (ihash_multimap::const_iterator i = query_params.begin(); i != query_params.end(); ++i) {
        if (i != query_params.begin())
            query_string += '&';
        query_string += algorithm::url_encode(i->first);
        query_string += '=';
        query_string += algorithm::url_encode(i->second);
    }
    return query_string;
}

std::string types::make_set_cookie_header(const std::string& name,
                                              const std::string& value,
                                              const std::string& path,
                                              const bool has_max_age,
                                              const unsigned long max_age)
{
    // note: according to RFC6265, attributes should not be quoted
    std::string set_cookie_header(name);
    set_cookie_header += "=\"";
    set_cookie_header += value;
    set_cookie_header += "\"; Version=1";
    if (! path.empty()) {
        set_cookie_header += "; Path=";
        set_cookie_header += path;
    }
    if (has_max_age) {
        set_cookie_header += "; Max-Age=";
        set_cookie_header += std::to_string(max_age);
    }
    return set_cookie_header;
}

    
}   // end namespace http
}   // end namespace pion

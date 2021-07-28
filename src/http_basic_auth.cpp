// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#include <pion/string_utils.hpp>
#include <pion/algorithm.hpp>
#include <pion/http/basic_auth.hpp>
#include <pion/http/response_writer.hpp>
#include <pion/http/server.hpp>


namespace pion {    // begin namespace pion
namespace http {    // begin namespace http
    
    
// static members of basic_auth

const unsigned int  basic_auth::CACHE_EXPIRATION = 300;  // 5 minutes


// basic_auth member functions

basic_auth::basic_auth(user_manager_ptr userManager, const std::string& realm)
    : http::auth(userManager), m_realm(realm),
	m_cache_cleanup_time(std::chrono::system_clock::now())
{
    set_logger(PION_GET_LOGGER("pion.http.basic_auth"));
}
    
bool basic_auth::handle_request(const http::request_ptr& http_request_ptr, const tcp::connection_ptr& tcp_conn)
{
    if (!need_authentication(http_request_ptr)) {
        return true; // this request does not require authentication
    }
    
	std::chrono::time_point<std::chrono::system_clock> time_now(std::chrono::system_clock::now());
    if (time_now > m_cache_cleanup_time + std::chrono::seconds(CACHE_EXPIRATION)) {
        // expire cache
        std::unique_lock<std::mutex> cache_lock(m_cache_mutex);
        user_cache_type::iterator i;
        user_cache_type::iterator next=m_user_cache.begin();
        while (next!=m_user_cache.end()) {
            i=next;
            ++next;
            if (time_now > i->second.first + std::chrono::seconds(CACHE_EXPIRATION)) {
                // ok - this is an old record.. expire it now
                m_user_cache.erase(i);
            }
        }
        m_cache_cleanup_time = time_now;
    }
    
    // if we are here, we need to check if access authorized...
    std::string authorization = http_request_ptr->get_header(http::types::HEADER_AUTHORIZATION);
    if (!authorization.empty()) {
        std::string credentials;
        if (parse_authorization(authorization, credentials)) {
            // to do - use fast cache to match with active credentials
            std::unique_lock<std::mutex> cache_lock(m_cache_mutex);
            user_cache_type::iterator user_cache_ptr=m_user_cache.find(credentials);
            if (user_cache_ptr!=m_user_cache.end()) {
                // we found the credentials in our cache...
                // we can approve authorization now!
                http_request_ptr->set_user(user_cache_ptr->second.second);
                user_cache_ptr->second.first = time_now;
                return true;
            }
    
            std::string username;
            std::string password;
    
            if (parse_credentials(credentials, username, password)) {
                // match username/password
                user_ptr user=m_user_manager->get_user(username, password);
                if (user) {
                    // add user to the cache
                    m_user_cache.insert(std::make_pair(credentials, std::make_pair(time_now, user)));
                    // add user credentials to the request object
                    http_request_ptr->set_user(user);
                    return true;
                }
            }
        }
    }

    // user not found
    handle_unauthorized(http_request_ptr, tcp_conn);
    return false;
}
    
void basic_auth::set_option(const std::string& name, const std::string& value) 
{
	if (name == "realm")
		m_realm = value;
	else
		std::cout << "bad_arg: " << name << std::endl;
}
    
bool basic_auth::parse_authorization(const std::string& authorization, std::string &credentials)
{
    if (!utils::starts_with(authorization, "Basic "))
        return false;
    credentials = authorization.substr(6);
    if (credentials.empty())
        return false;
    return true;
}
    
bool basic_auth::parse_credentials(const std::string &credentials,
    std::string &username, std::string &password)
{
    std::string user_password;
    
    if (! algorithm::base64_decode(credentials, user_password))
        return false;

    // find ':' symbol
    std::string::size_type i = user_password.find(':');
    if (i==0 || i==std::string::npos)
        return false;
    
    username = user_password.substr(0, i);
    password = user_password.substr(i+1);
    
    return true;
}
    
void basic_auth::handle_unauthorized(const http::request_ptr& http_request_ptr,
    const tcp::connection_ptr& tcp_conn)
{
    // authentication failed, send 401.....
    static const std::string CONTENT =
        " <!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\""
        "\"http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd\">"
        "<HTML>"
        "<HEAD>"
        "<TITLE>Error</TITLE>"
        "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=ISO-8859-1\">"
        "</HEAD>"
        "<BODY><H1>401 Unauthorized.</H1></BODY>"
        "</HTML> ";
    http::response_writer_ptr writer(http::response_writer::create(tcp_conn, *http_request_ptr,
                                                                   std::bind(&tcp::connection::finish, tcp_conn)));
    writer->get_response().set_status_code(http::types::RESPONSE_CODE_UNAUTHORIZED);
    writer->get_response().set_status_message(http::types::RESPONSE_MESSAGE_UNAUTHORIZED);
    writer->get_response().add_header("WWW-Authenticate", "Basic realm=\"" + m_realm + "\"");
    writer->write_no_copy(CONTENT);
    writer->send();
}
    
}   // end namespace http
}   // end namespace pion

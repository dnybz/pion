// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#include <pion/error.hpp>
#include <pion/http/plugin_server.hpp>
#include <pion/http/request.hpp>
#include <pion/http/basic_auth.hpp>
#include <pion/http/cookie_auth.hpp>
#include <fstream>


namespace pion {    // begin namespace pion
namespace http {    // begin namespace http


// plugin_server member functions

void plugin_server::add_service(const std::string& resource, http::plugin_service *service_ptr)
{
    plugin_ptr<http::plugin_service> plugin_ptr;
    const std::string clean_resource(strip_trailing_slash(resource));
    service_ptr->set_resource(clean_resource);
    m_services.add(clean_resource, service_ptr);
    http::server::add_resource(clean_resource, std::ref(*service_ptr));
    PION_LOG_INFO(m_logger, "Loaded static web service for resource (" << clean_resource << ")");
}

void plugin_server::load_service(const std::string& resource, const std::string& service_name)
{
    const std::string clean_resource(strip_trailing_slash(resource));
    http::plugin_service *service_ptr;
    service_ptr = m_services.load(clean_resource, service_name);
    http::server::add_resource(clean_resource, std::ref(*service_ptr));
    service_ptr->set_resource(clean_resource);
    PION_LOG_INFO(m_logger, "Loaded web service plug-in for resource (" << clean_resource << "): " << service_name);
}

void plugin_server::set_service_option(const std::string& resource,
                                 const std::string& name, const std::string& value)
{
    const std::string clean_resource(strip_trailing_slash(resource));
    m_services.run(clean_resource, std::bind(&http::plugin_service::set_option, std::placeholders::_1, name, value));
    PION_LOG_INFO(m_logger, "Set web service option for resource ("
                  << resource << "): " << name << '=' << value);
}

void plugin_server::load_service_config(const std::string& config_name)
{
    std::string config_file;
	if (!plugin::find_config_file(config_file, config_name)) {
		std::cout << "File not found: " << config_name << std::endl;
	}

    // open the file for reading
    std::ifstream config_stream;
    config_stream.open(config_file.c_str(), std::ios::in);
	if (!config_stream.is_open()) {
		std::cout << "Can't open file: " << config_name << std::endl;
	}
    
    // parse the contents of the file
    http::auth_ptr my_auth_ptr;
    enum ParseState {
        PARSE_NEWLINE, PARSE_COMMAND, PARSE_RESOURCE, PARSE_VALUE, PARSE_COMMENT, PARSE_USERNAME
    } parse_state = PARSE_NEWLINE;
    std::string command_string;
    std::string resource_string;
    std::string username_string;
    std::string value_string;
    std::string option_name_string;
    std::string option_value_string;
    int c = config_stream.get();    // read the first character
    
    while (config_stream) {
        switch(parse_state) {
        case PARSE_NEWLINE:
            // parsing command portion (or beginning of line)
            if (c == '#') {
                // line is a comment
                parse_state = PARSE_COMMENT;
            } else if (isalpha(c)) {
                // first char in command
                parse_state = PARSE_COMMAND;
                // ignore case for commands
                command_string += tolower(c);
            } else if (c != '\r' && c != '\n') {    // check for blank lines
				std::cout << "bad config: " << config_name << " blank lines" << std::endl;
            }
            break;
            
        case PARSE_COMMAND:
            // parsing command portion (or beginning of line)
            if (c == ' ' || c == '\t') {
                // command finished -> check if valid
                if (command_string=="path" || command_string=="auth" || command_string=="restrict") {
                    value_string.clear();
                    parse_state = PARSE_VALUE;
                } else if (command_string=="service" || command_string=="option") {
                    resource_string.clear();
                    parse_state = PARSE_RESOURCE;
                } else if (command_string=="user") {
                    username_string.clear();
                    parse_state = PARSE_USERNAME;
                } else {
					std::cout << "bad config: " << config_name << std::endl;
                }
            } else if (! isalpha(c)) {
                // commands may only contain alpha chars
				std::cout << "bad config: " << config_name << " only contain alpha chars." << std::endl;
            } else {
                // ignore case for commands
                command_string += tolower(c);
            }
            break;

        case PARSE_RESOURCE:
            // parsing resource portion (/hello)
            if (c == ' ' || c == '\t') {
                // check for leading whitespace
                if (! resource_string.empty()) {
                    // resource finished
                    value_string.clear();
                    parse_state = PARSE_VALUE;
                }
            } else if (c == '\r' || c == '\n') {
                // line truncated before value
				std::cout << "bad config: " << config_name << " line truncated before value" << std::endl;
            } else {
                // add char to resource
                resource_string += c;
            }
            break;
        
        case PARSE_USERNAME:
            // parsing username for user command
            if (c == ' ' || c == '\t') {
                // check for leading whitespace
                if (! username_string.empty()) {
                    // username finished
                    value_string.clear();
                    parse_state = PARSE_VALUE;
                }
            } else if (c == '\r' || c == '\n') {
                // line truncated before value (missing username)
				std::cout << "bad config: " << config_name << " line truncated before value (missing username)" << std::endl;
            } else {
                // add char to username
                username_string += c;
            }
            break;
        
        case PARSE_VALUE:
            // parsing value portion
            if (c == '\r' || c == '\n') {
                // value is finished
                if (value_string.empty()) {
                    // value must not be empty
					std::cout << "bad config: " << config_name << "value is empty" << std::endl;
                } else if (command_string == "path") {
                    // finished path command
                    try { plugin::add_plugin_directory(value_string); }
                    catch (std::exception& e) {
                        PION_LOG_WARN(m_logger, e.what());
                    }
                } else if (command_string == "auth") {
                    // finished auth command
                    user_manager_ptr user_mgr(new user_manager);
                    if (value_string == "basic"){
                        my_auth_ptr.reset(new http::basic_auth(user_mgr));
                    }
                    else if (value_string == "cookie"){
                        my_auth_ptr.reset(new http::cookie_auth(user_mgr));
                    }
                    else {
                        // only basic and cookie authentications are supported
                        std::cout << "bad config: " << config_name << "Not support name: " << command_string << "value: " << value_string << std::endl;
                    }
                } else if (command_string == "restrict") {
                    // finished restrict command
                    if (! my_auth_ptr)
                        // Authentication type must be defined before restrict
                        std::cout << "bad config: " << config_name << " Authentication type must be defined before restrict" << std::endl;
                    else if (value_string.empty())
                        // No service defined for restrict parameter
                        std::cout << "bad config: " << config_name << " No service defined for restrict parameter" << std::endl;
                    my_auth_ptr->add_restrict(value_string);
                } else if (command_string == "user") {
                    // finished user command
					if (!my_auth_ptr) {
						// Authentication type must be defined before users
						std::cout << "bad config: " << config_name << " Authentication type must be defined before users" << std::endl;
					}
					else if (value_string.empty()) {
						// No password defined for user parameter
						std::cout << "bad config: " << config_name << " No password defined for user parameter" << std::endl;
					}
                    my_auth_ptr->add_user(username_string, value_string);
                } else if (command_string == "service") {
                    // finished service command
                    load_service(resource_string, value_string);
                } else if (command_string == "option") {
                    // finished option command
                    std::string::size_type pos = value_string.find('=');
					if (pos == std::string::npos) {
						std::cout << "bad config: " << config_name << " " << value_string << std::endl;
					}
                    option_name_string = value_string.substr(0, pos);
                    option_value_string = value_string.substr(pos + 1);
                    set_service_option(resource_string, option_name_string,
                                     option_value_string);
                }
                command_string.clear();
                parse_state = PARSE_NEWLINE;
            } else if (c == ' ' || c == '\t') {
                // only skip leading whitespace (value may contain spaces, etc)
                if (! value_string.empty())
                    value_string += c;
            } else {
                // add char to value
                value_string += c;
            }
            break;
        
        case PARSE_COMMENT:
            // skipping comment line
            if (c == '\r' || c == '\n')
                parse_state = PARSE_NEWLINE;
            break;
        }

        // read the next character
        c = config_stream.get();
    }
    
    // update authentication configuration for the server
    set_authentication(my_auth_ptr);
}

}   // end namespace http
}   // end namespace pion

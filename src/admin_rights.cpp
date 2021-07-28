// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#include <pion/admin_rights.hpp>
#include <pion/string_utils.hpp>

#ifndef PION_WIN32
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <regex>
    #include <fstream>
#endif


namespace pion {    // begin namespace pion


// static members of admin_rights

const int16_t    admin_rights::ADMIN_USER_ID = 0;
std::mutex            admin_rights::m_mutex;


// admin_rights member functions

#ifdef PION_WIN32

admin_rights::admin_rights(bool use_log)
    : m_logger(PION_GET_LOGGER("pion.admin_rights")),
    m_lock(m_mutex), m_user_id(-1), m_has_rights(false), m_use_log(use_log)
{}

void admin_rights::release(void)
{}

long admin_rights::run_as_user(const std::string& /* user_name */)
{
    return -1;
}

long admin_rights::run_as_group(const std::string& /* group_name */)
{
    return -1;
}

long admin_rights::find_system_id(const std::string& /* name */,
    const std::string& /* file */)
{
    return -1;
}

#else   // NOT #ifdef PION_WIN32

admin_rights::admin_rights(bool use_log)
    : m_logger(PION_GET_LOGGER("pion.admin_rights")),
    m_lock(m_mutex), m_user_id(-1), m_has_rights(false), m_use_log(use_log)
{
    m_user_id = geteuid();
    if ( seteuid(ADMIN_USER_ID) != 0 ) {
        if (m_use_log)
            PION_LOG_ERROR(m_logger, "Unable to upgrade to administrative rights");
        m_lock.unlock();
        return;
    } else {
        m_has_rights = true;
        if (m_use_log)
            PION_LOG_DEBUG(m_logger, "Upgraded to administrative rights");
    }
}

void admin_rights::release(void)
{
    if (m_has_rights) {
        if ( seteuid(m_user_id) == 0 ) {
            if (m_use_log)
                PION_LOG_DEBUG(m_logger, "Released administrative rights");
        } else {
            if (m_use_log)
                PION_LOG_ERROR(m_logger, "Unable to release administrative rights");
        }
        m_has_rights = false;
        m_lock.unlock();
    }
}

long admin_rights::run_as_user(const std::string& user_name)
{
    long user_id = find_system_id(user_name, "/etc/passwd");
    if (user_id != -1) {
        if ( seteuid(user_id) != 0 )
            user_id = -1;
    } else {
        user_id = geteuid();
    }
    return user_id;
}

long admin_rights::run_as_group(const std::string& group_name)
{
    long group_id = find_system_id(group_name, "/etc/group");
    if (group_id != -1) {
        if ( setegid(group_id) != 0 )
            group_id = -1;
    } else {
        group_id = getegid();
    }
    return group_id;
}

long admin_rights::find_system_id(const std::string& name,
    const std::string& file)
{
    // check if name is the system id
    const std::regex just_numbers("\\d+");
    if (std::regex_match(name, just_numbers)) {
        return std::stoi(name);
    }

    // open system file
    std::ifstream system_file(file.c_str());
    if (! system_file.is_open()) {
        return -1;
    }

    // find id in system file
    std::string line;
    int32_t system_id = -1;

    while (std::getline(system_file, line, '\n')) {
		std::vector<std::string> tokens = utils::split(line, ':');
		std::vector<std::string>::const_iterator token_it = tokens.begin();
        if (token_it != tokens.end() && *token_it == name) {
            // found line matching name
            if (++token_it != tokens.end() && ++token_it != tokens.end()
                && std::regex_match(*token_it, just_numbers))
            {
                // found id as third parameter
                system_id = std::stoi(*token_it);
            }
            break;
        }
    }

    return system_id;
}

#endif  // #ifdef PION_WIN32
    
}   // end namespace pion


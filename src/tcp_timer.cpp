// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#include <pion/tcp/timer.hpp>
#include <memory>

namespace pion {    // begin namespace pion
namespace tcp {     // begin namespace tcp


// timer member functions

timer::timer(const tcp::connection_ptr& conn_ptr)
    : m_conn_ptr(conn_ptr), m_timer(conn_ptr->get_io_service()),
    m_timer_active(false), m_was_cancelled(false)
{
}

void timer::start(const uint32_t seconds)
{
    std::unique_lock<std::mutex> timer_lock(m_mutex);
    m_timer_active = true;
    m_timer.expires_from_now(std::chrono::seconds(seconds));
    m_timer.async_wait(std::bind(&timer::timer_callback,
        shared_from_this(), std::placeholders::_1));
}

void timer::cancel(void)
{
    std::unique_lock<std::mutex> timer_lock(m_mutex);
    m_was_cancelled = true;
    if (m_timer_active)
        m_timer.cancel();
}

void timer::timer_callback(const asio::error_code& /* ec */)
{
    std::unique_lock<std::mutex> timer_lock(m_mutex);
    m_timer_active = false;
    if (! m_was_cancelled)
        m_conn_ptr->cancel();
}


}   // end namespace tcp
}   // end namespace pion

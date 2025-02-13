// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#include <pion/scheduler.hpp>

namespace pion {    // begin namespace pion


// static members of scheduler
    
const uint32_t   scheduler::DEFAULT_NUM_THREADS = 8;
const uint32_t   scheduler::NSEC_IN_SECOND = 1000000000; // (10^9)
const uint32_t   scheduler::MICROSEC_IN_SECOND = 1000000;    // (10^6)
const uint32_t   scheduler::KEEP_RUNNING_TIMER_SECONDS = 5;


// scheduler member functions

void scheduler::shutdown(void)
{
    // lock mutex for thread safety
    std::unique_lock<std::mutex> scheduler_lock(m_mutex);
    
    if (m_is_running) {
        
        PION_LOG_INFO(m_logger, "Shutting down the thread scheduler");
        
        while (m_active_users > 0) {
            // first, wait for any active users to exit
            PION_LOG_INFO(m_logger, "Waiting for " << m_active_users << " scheduler users to finish");
            m_no_more_active_users.wait(scheduler_lock);
        }

        // shut everything down
        m_is_running = false;
        stop_services();
        stop_threads();
        finish_services();
        finish_threads();
        
        PION_LOG_INFO(m_logger, "The thread scheduler has shutdown");

        // Make sure anyone waiting on shutdown gets notified
        m_scheduler_has_stopped.notify_all();
        
    } else {
        
        // stop and finish everything to be certain that no events are pending
        stop_services();
        stop_threads();
        finish_services();
        finish_threads();
        
        // Make sure anyone waiting on shutdown gets notified
        // even if the scheduler did not startup successfully
        m_scheduler_has_stopped.notify_all();
    }
}

void scheduler::join(void)
{
    std::unique_lock<std::mutex> scheduler_lock(m_mutex);
    while (m_is_running) {
        // sleep until scheduler_has_stopped condition is signaled
        m_scheduler_has_stopped.wait(scheduler_lock);
    }
}
    
void scheduler::keep_running(asio::io_service& my_service,
                                asio::steady_timer& my_timer)
{
    if (m_is_running) {
        // schedule this again to make sure the service doesn't complete
        my_timer.expires_from_now(std::chrono::seconds(KEEP_RUNNING_TIMER_SECONDS));
        my_timer.async_wait(std::bind(&scheduler::keep_running, this,
                                        std::ref(my_service), std::ref(my_timer)));
    }
}

void scheduler::add_active_user(void)
{
    if (!m_is_running) startup();
    std::unique_lock<std::mutex> scheduler_lock(m_mutex);
    ++m_active_users;
}

void scheduler::remove_active_user(void)
{
    std::unique_lock<std::mutex> scheduler_lock(m_mutex);
    if (--m_active_users == 0)
        m_no_more_active_users.notify_all();
}
              
void scheduler::process_service_work(asio::io_service& service) {
    while (m_is_running) {
        try {
            service.run();
        } catch (std::exception& e) {
            PION_LOG_ERROR(m_logger, e.what());
        } catch (...) {
            PION_LOG_ERROR(m_logger, "caught unrecognized exception");
        }
    }   
}
                     

// single_service_scheduler member functions

void single_service_scheduler::startup(void)
{
    // lock mutex for thread safety
    std::unique_lock<std::mutex> scheduler_lock(m_mutex);
    
    if (! m_is_running) {
        PION_LOG_INFO(m_logger, "Starting thread scheduler");
        m_is_running = true;
        
        // schedule a work item to make sure that the service doesn't complete
        m_service.reset();
        keep_running(m_service, m_timer);
        
        // start multiple threads to handle async tasks
        for (uint32_t n = 0; n < m_num_threads; ++n) {
            std::shared_ptr<std::thread> new_thread(new std::thread( std::bind(&scheduler::process_service_work,
                                                                                       this, std::ref(m_service)) ));
            m_thread_pool.push_back(new_thread);
        }
    }
}

    
// one_to_one_scheduler member functions

void one_to_one_scheduler::startup(void)
{
    // lock mutex for thread safety
    std::unique_lock<std::mutex> scheduler_lock(m_mutex);
    
    if (! m_is_running) {
        PION_LOG_INFO(m_logger, "Starting thread scheduler");
        m_is_running = true;
        
        // make sure there are enough services initialized
        while (m_service_pool.size() < m_num_threads) {
            std::shared_ptr<service_pair_type>  service_ptr(new service_pair_type());
            m_service_pool.push_back(service_ptr);
        }

        // schedule a work item for each service to make sure that it doesn't complete
        for (service_pool_type::iterator i = m_service_pool.begin(); i != m_service_pool.end(); ++i) {
            keep_running((*i)->first, (*i)->second);
        }
        
        // start multiple threads to handle async tasks
        for (uint32_t n = 0; n < m_num_threads; ++n) {
            std::shared_ptr<std::thread> new_thread(new std::thread( std::bind(&scheduler::process_service_work,
                                                                                       this, std::ref(m_service_pool[n]->first)) ));
            m_thread_pool.push_back(new_thread);
        }
    }
}

    
}   // end namespace pion

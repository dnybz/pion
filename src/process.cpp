// ---------------------------------------------------------------------
// pion:  a Boost C++ framework for building lightweight HTTP interfaces
// ---------------------------------------------------------------------
// Copyright (C) 2021 Wang Qiang  (https://github.com/dnybz/pion)
// Copyright (C) 2007-2014 Splunk Inc.  (https://github.com/splunk/pion)
//
// Distributed under the Boost Software License, Version 1.0.
// See http://www.boost.org/LICENSE_1_0.txt
//

#include <pion/config.hpp>
#include <pion/process.hpp>
#include <pion/logger.hpp>
#include <sstream>
#include <iostream>
#include <xiosbase>
#include <signal.h>
#include <time.h>
#ifndef PION_WIN32
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
#endif

namespace pion {    // begin namespace pion
    
// static members of process
    
std::once_flag process::m_instance_flag;
process::config_type *process::m_config_ptr = NULL;

    
// process member functions
    
void process::shutdown(void)
{
    config_type& cfg = get_config();
    std::unique_lock<std::mutex> shutdown_lock(cfg.shutdown_mutex);
    if (! cfg.shutdown_now) {
        cfg.shutdown_now = true;
        cfg.shutdown_cond.notify_all();
    }
}

void process::wait_for_shutdown(void)
{
    config_type& cfg = get_config();
    std::unique_lock<std::mutex> shutdown_lock(cfg.shutdown_mutex);
    while (! cfg.shutdown_now)
        cfg.shutdown_cond.wait(shutdown_lock);
}

void process::create_config(void)
{
    static config_type UNIQUE_PION_PROCESS_CONFIG;
    m_config_ptr = &UNIQUE_PION_PROCESS_CONFIG;
}



#ifdef PION_WIN32

BOOL WINAPI console_ctrl_handler(DWORD ctrl_type)
{
    switch(ctrl_type) {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
        case CTRL_SHUTDOWN_EVENT:
            process::shutdown();
            return TRUE;
        default:
            return FALSE;
    }
}

void process::set_dumpfile_directory(const std::string& dir)
{
    config_type& cfg = get_config();
    static const TCHAR* DBGHELP_DLL = _T("DBGHELP.DLL");

    if (!dir.empty() && !fs::is_directory(dir)) {
        throw dumpfile_init_exception("Dump file directory doesn't exist: " + dir);
    }

    cfg.dumpfile_dir = dir;

    // load dbghelp.dll
    if (!dir.empty()) {
        HMODULE hDll = NULL;
        TCHAR szDbgHelpPath[_MAX_PATH];

        // try loading side-by-side version of DbgHelp.dll first
        if (GetModuleFileName(NULL, szDbgHelpPath, _MAX_PATH)) {
            TCHAR *pSlash = _tcsrchr(szDbgHelpPath, _T('\\'));
            if (pSlash) {
                _tcscpy(pSlash+1, DBGHELP_DLL);
                hDll = ::LoadLibrary( szDbgHelpPath );
            }
        }
        // if not found, load the default version
        if (hDll == NULL) {
            hDll = ::LoadLibrary( DBGHELP_DLL );
        }
        cfg.h_dbghelp = hDll;

        if (hDll == NULL) {
            throw dumpfile_init_exception("Failed to load DbgHelp.dll");
        }
    } else {
        cfg.h_dbghelp = NULL;
    }

    // get MiniDumpWriteDump proc address
    if (cfg.h_dbghelp != NULL) {
        cfg.p_dump_proc = (MINIDUMPWRITEDUMP)::GetProcAddress(cfg.h_dbghelp, "MiniDumpWriteDump");

        if (cfg.p_dump_proc == NULL) {
            throw dumpfile_init_exception("Failed to get MiniDumpWriteDump proc address, probably dbghelp.dll version is too old");
        }
    } else {
        cfg.p_dump_proc = NULL;
    }

    pion::logger _logger = PION_GET_LOGGER("pion.process");
    // (re)set the exception filter
    if (cfg.p_dump_proc) {
        ::SetUnhandledExceptionFilter(process::unhandled_exception_filter);
         PION_LOG_INFO(_logger, "Dump file generation enabled to " << cfg.dumpfile_dir );
    } else {
        ::SetUnhandledExceptionFilter(NULL);
        PION_LOG_INFO(_logger, "Unhandled exception handling reset to default");
    }
}

std::string process::generate_dumpfile_name()
{
    config_type& cfg = get_config();

    // generate file name based on current timestamp

	tm _tm;
	time_t t = time(NULL);
	localtime_s(&_tm, &t);
	char buffer[64] = {0};
	strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &_tm);
    std::stringstream ss;
    ss << buffer << ".dmp";

    // build the full path
    fs::path p(cfg.dumpfile_dir);

    p /= ss.str();
    p.make_preferred();

    return p.string();

}

LONG WINAPI process::unhandled_exception_filter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
    config_type& cfg = get_config();
    pion::logger _logger = PION_GET_LOGGER("pion.process");
    
    // make sure we have all the necessary setup
    if (cfg.dumpfile_dir.empty() || cfg.p_dump_proc == NULL) {
        PION_LOG_FATAL(_logger, "Unhandled exception caught when dump file handling not configured!");
        PION_SHUTDOWN_LOGGER;
        return EXCEPTION_CONTINUE_SEARCH;
    }

    std::string dumpfile_path = generate_dumpfile_name();
    LONG rc = EXCEPTION_CONTINUE_SEARCH;

    // create the dump file and, if successful, write it
    HANDLE hFile = ::CreateFileA(dumpfile_path.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile!=INVALID_HANDLE_VALUE) {
        _MINIDUMP_EXCEPTION_INFORMATION ExInfo;

        ExInfo.ThreadId = ::GetCurrentThreadId();
        ExInfo.ExceptionPointers = pExceptionInfo;
        ExInfo.ClientPointers = 0;

        // write the dump
        BOOL bOK = cfg.p_dump_proc(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );

        if (bOK) {
            PION_LOG_INFO(_logger, "Saved process dump file to " << dumpfile_path);
        } else {
            PION_LOG_ERROR(_logger, "Failed to save dump file to " << dumpfile_path << 
                " error code: " << GetLastError());
        }

        ::CloseHandle(hFile);
        rc = EXCEPTION_EXECUTE_HANDLER; // dump saved, so we can die peacefully..
    } else {
        PION_LOG_ERROR(_logger, "Failed to create dump file " << dumpfile_path << 
            " error code: " << GetLastError());
    }

    PION_LOG_FATAL(_logger, "Unhandled exception caught. The process will be terminated!");
    PION_SHUTDOWN_LOGGER;
    return rc;
}


void process::initialize(void)
{
    SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
}

void process::daemonize(void)
{
    // not supported
}

#else   // NOT #ifdef PION_WIN32

void handle_signal(int /* sig */)
{
    process::shutdown();
}

void process::initialize(void)
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
}

void process::daemonize(void)
{
    // adopted from "Unix Daemon Server Programming"
    // http://www.enderunix.org/docs/eng/daemon.php
    
    // return early if already running as a daemon
    if(getppid()==1) return;
    
    // for out the process 
    int i = fork();
    if (i<0) exit(1);   // error forking
    if (i>0) exit(0);   // exit if parent
    
    // child (daemon process) continues here after the fork...
    
    // obtain a new process group
    setsid();
    
    // close all descriptors
    for (i=getdtablesize();i>=0;--i) close(i);
    
    // bind stdio to /dev/null (ignore errors)
    i=open("/dev/null",O_RDWR);
    if (i != -1) {
        if (dup(i) == -1) {}
        if (dup(i) == -1) {}
    }
    
    // restrict file creation mode to 0750
    umask(027);
}

#endif  // #ifdef PION_WIN32

}   // end namespace pion

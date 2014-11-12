/*
 * Copyright (C) 2012-2014  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *          Marco Randazzo          <marco.randazzo@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/os/Log.h>
#include <yarp/os/impl/LogImpl.h>
#include <yarp/os/impl/LogForwarder.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <cstdlib>
#include <cstring>
#include <cstdarg>

#ifdef YARP_HAS_ACE
# include <ace/Stack_Trace.h>
#elif defined(YARP_HAS_EXECINFO)
# include <execinfo.h>
#endif

#include <yarp/conf/system.h>
#include <yarp/os/Os.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/impl/PlatformStdio.h>


#define YARP_MAX_LOG_MSG_SIZE 512

#ifndef WIN32

 #define RED     (yarp::os::impl::LogImpl::colored_output ? "\033[01;31m" : "")
 #define GREEN   (yarp::os::impl::LogImpl::colored_output ? "\033[01;32m" : "")
 #define YELLOW  (yarp::os::impl::LogImpl::colored_output ? "\033[01;33m" : "")
 #define BLUE    (yarp::os::impl::LogImpl::colored_output ? "\033[01;34m" : "")
 #define MAGENTA (yarp::os::impl::LogImpl::colored_output ? "\033[01;35m" : "")
 #define CYAN    (yarp::os::impl::LogImpl::colored_output ? "\033[01;36m" : "")
 #define WHITE   (yarp::os::impl::LogImpl::colored_output ? "\033[01;37m" : "")
 #define RED_BG  (yarp::os::impl::LogImpl::colored_output ? "\033[01;41m" : "")
 #define CLEAR   (yarp::os::impl::LogImpl::colored_output ? "\033[00m" : "")

#else // WIN32

 // TODO colored and verbose_output for WIN32
 #define RED     ""
 #define GREEN   ""
 #define YELLOW  ""
 #define BLUE    ""
 #define MAGENTA ""
 #define CYAN    ""
 #define WHITE   ""
 #define RED_BG  ""
 #define CLEAR   ""

#endif // WIN32

static bool from_env(const char* name, bool defaultvalue) {
    const char *strvalue = yarp::os::getenv(name);

    if(!strvalue) return defaultvalue;

    if(strcmp(strvalue, "1") == 0) return true;
    if(strcmp(strvalue, "true") == 0) return true;
    if(strcmp(strvalue, "True") == 0) return true;
    if(strcmp(strvalue, "TRUE") == 0) return true;
    if(strcmp(strvalue, "on") == 0) return true;
    if(strcmp(strvalue, "On") == 0) return true;
    if(strcmp(strvalue, "ON") == 0) return true;

    if(strcmp(strvalue, "0") == 0) return false;
    if(strcmp(strvalue, "false") == 0) return false;
    if(strcmp(strvalue, "False") == 0) return false;
    if(strcmp(strvalue, "FALSE") == 0) return false;
    if(strcmp(strvalue, "off") == 0) return false;
    if(strcmp(strvalue, "Off") == 0) return false;
    if(strcmp(strvalue, "OFF") == 0) return false;

    return defaultvalue;
}

bool yarp::os::impl::LogImpl::colored_output(from_env("YARP_COLORED_OUTPUT", false));
bool yarp::os::impl::LogImpl::verbose_output(from_env("YARP_VERBOSE_OUTPUT", false));
bool yarp::os::impl::LogImpl::debug_output(from_env("YARP_DEBUG_ENABLE", true));
bool yarp::os::impl::LogImpl::trace_output(from_env("YARP_TRACE_ENABLE", false) && yarp::os::impl::LogImpl::debug_output);
bool yarp::os::impl::LogImpl::forward_output(from_env("YARP_FORWARD_LOG_ENABLE", false));

yarp::os::Log::LogCallback yarp::os::Log::print_callback = yarp::os::impl::LogImpl::print_callback;
yarp::os::Log::LogCallback yarp::os::Log::forward_callback = yarp::os::impl::LogImpl::forward_callback;

yarp::os::impl::LogImpl::LogImpl(const char *file,
                                 unsigned int line,
                                 const char *func) :
        file(file),
        line(line),
        func(func)
{
}



void yarp::os::impl::LogImpl::print_callback(yarp::os::Log::LogType t,
                                             const char *msg,
                                             const char *file,
                                             const unsigned int line,
                                             const char *func)
{
    if (t == yarp::os::Log::TraceType && !trace_output) {
        return;
    }
    if (t == yarp::os::Log::DebugType && !debug_output) {
        return;
    }

    std::ostream *ost;
    if(t == yarp::os::Log::TraceType ||
       t == yarp::os::Log::DebugType ||
       t == yarp::os::Log::InfoType) {
        ost = &std::cout;
    } else {
        ost = &std::cerr;
    }

    const char *color = "";
    const char *bgcolor = "";
    const char *tag = "";

    switch (t) {
    case yarp::os::Log::TraceType:
        color = WHITE;
        tag = "TRACE";
        break;
    case yarp::os::Log::DebugType:
        color = GREEN;
        tag = "DEBUG";
        break;
    case yarp::os::Log::InfoType:
        color = BLUE;
        tag = "INFO";
        break;
    case yarp::os::Log::WarningType:
        color = YELLOW;
        tag = "WARNING";
        break;
    case yarp::os::Log::ErrorType:
        color = RED;
        tag = "ERROR";
        break;
    case yarp::os::Log::FatalType:
        color = WHITE;
        bgcolor = RED_BG;
        tag = "FATAL";
        break;
    default:
        break;
    }

    *ost << "[" << color << bgcolor << tag << CLEAR << "]";

    if (verbose_output) {
        *ost << file << ":" << line << " " << color << bgcolor << func << CLEAR;
        *ost << (msg[0] ? ": " : "");
    } else if (t == yarp::os::Log::TraceType) {
        *ost << WHITE << func << CLEAR << (msg[0] ? ": " : "");
    }
    *ost << msg << std::endl;
}

void yarp::os::impl::LogImpl::forward_callback(yarp::os::Log::LogType t,
                                               const char *msg,
                                               const char *file,
                                               const unsigned int line,
                                               const char *func)
{
    if (!forward_output) {
        return;
    }
    if (t == yarp::os::Log::TraceType && !trace_output) {
        return;
    }
    if (t == yarp::os::Log::DebugType && !debug_output) {
        return;
    }

    std::stringstream stringstream_buffer;
    LogForwarder* theForwarder = LogForwarder::getInstance();

    const char *tag = "";

    switch (t) {
    case yarp::os::Log::TraceType:
        tag = "TRACE";
        break;
    case yarp::os::Log::DebugType:
        tag = "DEBUG";
        break;
    case yarp::os::Log::InfoType:
        tag = "INFO";
        break;
    case yarp::os::Log::WarningType:
        tag = "WARNING";
        break;
    case yarp::os::Log::ErrorType:
        tag = "ERROR";
        break;
    case yarp::os::Log::FatalType:
        tag = "FATAL";
        break;
    default:
        break;
    }

    stringstream_buffer << "[" << tag << "]";

    if (verbose_output) {
        stringstream_buffer << file << ":" << line << " " << func;
        stringstream_buffer << (msg[0] ? ": " : "");
    } else if (t == yarp::os::Log::TraceType) {
        stringstream_buffer << func << (msg[0] ? ": " : "");
    }
    stringstream_buffer << msg << std::endl;

    theForwarder->forward(stringstream_buffer.str());
}

yarp::os::Log::Log(const char *file,
                   unsigned int line,
                   const char *func) :
        mPriv(new yarp::os::impl::LogImpl(file, line, func))
{
}

yarp::os::Log::Log() :
        mPriv(new yarp::os::impl::LogImpl(NULL, 0, NULL))
{
}

yarp::os::Log::~Log()
{
    delete mPriv;
}


void yarp::os::Log::trace(const char *msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w =ACE_OS::vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        if (w>0 && buf[w-1]=='\n') {
            buf[w-1]=0;
        }
        if (print_callback) {
            print_callback(yarp::os::Log::TraceType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback) {
            forward_callback(yarp::os::Log::TraceType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::trace() const
{
    return yarp::os::LogStream(yarp::os::Log::TraceType, mPriv->file, mPriv->line, mPriv->func);
}


void yarp::os::Log::debug(const char *msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = ACE_OS::vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        if (w>0 && buf[w-1]=='\n') {
            buf[w-1]=0;
        }
        if (print_callback) {
            print_callback(yarp::os::Log::DebugType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback) {
            forward_callback(yarp::os::Log::DebugType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::debug() const
{
    return yarp::os::LogStream(yarp::os::Log::DebugType, mPriv->file, mPriv->line, mPriv->func);
}


void yarp::os::Log::info(const char *msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = ACE_OS::vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        if (w>0 && buf[w-1]=='\n') {
            buf[w-1]=0;
        }
        if (print_callback) {
            print_callback(yarp::os::Log::InfoType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback) {
            forward_callback(yarp::os::Log::InfoType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::info() const
{
    return yarp::os::LogStream(yarp::os::Log::InfoType, mPriv->file, mPriv->line, mPriv->func);
}


void yarp::os::Log::warning(const char *msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = ACE_OS::vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        if (w>0 && buf[w-1]=='\n') {
            buf[w-1]=0;
        }
        if (print_callback) {
            print_callback(yarp::os::Log::WarningType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback) {
            forward_callback(yarp::os::Log::WarningType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::warning() const
{
    return yarp::os::LogStream(yarp::os::Log::WarningType, mPriv->file, mPriv->line, mPriv->func);
}


void yarp::os::Log::error(const char *msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = ACE_OS::vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        if (w>0 && buf[w-1]=='\n') {
            buf[w-1]=0;
        }
        if (print_callback) {
            print_callback(yarp::os::Log::ErrorType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback) {
            forward_callback(yarp::os::Log::ErrorType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
    }
    va_end(args);
}

yarp::os::LogStream yarp::os::Log::error() const
{
    return yarp::os::LogStream(yarp::os::Log::ErrorType, mPriv->file, mPriv->line, mPriv->func);
}



void yarp::os::Log::fatal(const char *msg, ...) const
{
    va_list args;
    va_start(args, msg);
    if (msg) {
        char buf[YARP_MAX_LOG_MSG_SIZE];
        int w = ACE_OS::vsnprintf(buf, YARP_MAX_LOG_MSG_SIZE, msg, args);
        if (w>0 && buf[w-1]=='\n') {
            buf[w-1]=0;
        }
        if (print_callback) {
            print_callback(yarp::os::Log::FatalType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
        if (forward_callback) {
            forward_callback(yarp::os::Log::FatalType, buf, mPriv->file, mPriv->line, mPriv->func);
        }
    }
    va_end(args);
    yarp_print_trace(stderr, mPriv->file, mPriv->line);
    yarp::os::exit(-1);
}

yarp::os::LogStream yarp::os::Log::fatal() const
{
    return yarp::os::LogStream(yarp::os::Log::FatalType, mPriv->file, mPriv->line, mPriv->func);
}

void yarp::os::Log::setLogCallback(yarp::os::Log::LogCallback cb)
{
    print_callback = cb;
}

void yarp_print_trace(FILE *out, const char *file, int line) {
#ifdef YARP_HAS_ACE
    ACE_Stack_Trace st(-1);
    // TODO demangle symbols using <cxxabi.h> and abi::__cxa_demangle
    //      when available.
    ACE_OS::fprintf(out, "%s", st.c_str());
#elif defined(YARP_HAS_EXECINFO)
    const size_t max_depth = 100;
    size_t stack_depth;
    void *stack_addrs[max_depth];
    char **stack_strings;
    stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);
    fprintf(out, "Assertion thrown at %s:%d by code called from:\n", file, line);
    for (size_t i = 1; i < stack_depth; i++) {
        fprintf(out, " --> %s\n", stack_strings[i]);
    }
    free(stack_strings); // malloc()ed by backtrace_symbols
    fflush(out);
#else
    // Not implemented on this platform
#endif
}

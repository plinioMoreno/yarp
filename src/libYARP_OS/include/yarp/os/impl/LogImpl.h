/*
 * Copyright (C) 2012-2014  iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *          Marco Randazzo          <marco.randazzo@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef _YARP2_LOGIMPL_
#define _YARP2_LOGIMPL_

#include <yarp/os/Log.h>

class yarp::os::impl::LogImpl
{
public:
    LogImpl(const char *file,
            const unsigned int line,
            const char *func,
            const char *comp);

    void log(yarp::os::Log::LogType type,
             bool forward,
             const char *msg,
             va_list args) const;

    static void print_callback(yarp::os::Log::LogType t,
                               const char *msg,
                               const char *file,
                               const unsigned int line,
                               const char *func,
                               const char *comp);

    static void forward_callback(yarp::os::Log::LogType t,
                                 const char *msg,
                                 const char *file,
                                 const unsigned int line,
                                 const char *func,
                                 const char *comp);

    const char *file;
    const unsigned int line;
    const char *func;
    const char *comp;

    static bool colored_output;
    static bool verbose_output;
    static bool debug_output;
    static bool trace_output;
    static bool forward_output;
};


#endif // _YARP2_LOGIMPL_

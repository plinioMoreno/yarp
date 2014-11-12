/*
 * Copyright (C) 2012-2014  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#include <yarp/os/Log.h>

#include <yarp/os/impl/UnitTest.h>

#define LOG_COMPONENT "LogTest"

class LogTest : public yarp::os::impl::UnitTest {
public:
    virtual yarp::os::impl::String getName() { return "LogTest"; }

    void checkLog() {
        int i = 13;

        yTrace("This is a trace");
        yTrace("This is %s (%d)", "a trace", i);
        yTrace("The end of line is removed from this trace\n");
        yTraceNoFw("This trace is not forwarded");
        yCTrace(LOG_COMPONENT, "This is a trace");
        yCTrace(LOG_COMPONENT, "This is %s (%d)", "a trace", i);
        yCTrace(LOG_COMPONENT, "The end of line is removed from this trace\n");

        yDebug("This is a debug");
        yDebug("This is %s (%d)", "a debug", i);
        yDebug("The end of line is removed from this debug\n");
        yDebugNoFw("This debug is not forwarded");
        yCDebug(LOG_COMPONENT, "This is a debug");
        yCDebug(LOG_COMPONENT, "This is %s (%d)", "a debug", i);
        yCDebug(LOG_COMPONENT, "The end of line is removed from this debug\n");
        yCDebugNoFw(LOG_COMPONENT, "This debug is not forwarded");

        yInfo("This is info");
        yInfo("This is %s (%d)", "info", i);
        yInfo("The end of line is removed from this info\n");
        yInfoNoFw("This info is not forwarded");
        yCInfo(LOG_COMPONENT, "This is info");
        yCInfo(LOG_COMPONENT, "This is %s (%d)", "info", i);
        yCInfo(LOG_COMPONENT, "The end of line is removed from this info\n");
        yCInfoNoFw(LOG_COMPONENT, "This info is not forwarded");

        yWarning("This is a warning");
        yWarning("This is %s (%d)", "a warning", i);
        yWarning("The end of line is removed from this warning\n");
        yWarningNoFw("This warning is not forwarded");
        yCWarning(LOG_COMPONENT, "This is a warning");
        yCWarning(LOG_COMPONENT, "This is %s (%d)", "a warning", i);
        yCWarning(LOG_COMPONENT, "The end of line is removed from this warning\n");
        yCWarningNoFw(LOG_COMPONENT, "This warning is not forwarded");

        yError("This is an error");
        yError("This is %s (%d)", "an error", i);
        yError("The end of line is removed from this error\n");
        yErrorNoFw("This error is not forwarded");
        yCError(LOG_COMPONENT, "This is an error");
        yCError(LOG_COMPONENT, "This is %s (%d)", "an error", i);
        yCError(LOG_COMPONENT, "The end of line is removed from this error\n");
        yCErrorNoFw(LOG_COMPONENT, "This error is not forwarded");
    }

    virtual void runTests() {
        checkLog();
    }
};

static LogTest theLogTest;

yarp::os::impl::UnitTest& getLogTest() {
    return theLogTest;
}

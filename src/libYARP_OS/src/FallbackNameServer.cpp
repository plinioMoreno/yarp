// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>
#ifdef YARP_HAS_ACE

#include <yarp/os/impl/FallbackNameServer.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/NetType.h>
#include <yarp/os/impl/NameServer.h>
#include <yarp/os/Bytes.h>

using namespace yarp::os::impl;
using namespace yarp::os;

Contact FallbackNameServer::getAddress() {
    Contact mcastLastResort = Contact::bySocket("mcast","224.2.1.1",NetworkBase::getDefaultPortRange()).addName("fallback");
    return mcastLastResort;
}


void FallbackNameServer::run() {
    DgramTwoWayStream send;
    send.join(getAddress(),true);
    listen.join(getAddress(),false);

    yDebugNoFw("Fallback server running");
    while (listen.isOk()&&send.isOk()&&!closed) {
        yDebugNoFw("Fallback server waiting");
        String msg;
        listen.beginPacket();
        msg = listen.readLine();
        listen.endPacket();
        yDebugNoFw("Fallback server got something");
        if (listen.isOk()&&!closed) {
            yDebugNoFw("Fallback server got %s", msg.c_str());
            if (msg.find("NAME_SERVER ") == 0) {
                Contact addr;
                String result = owner.apply(msg,addr);
                send.beginPacket();
                send.writeLine(result.c_str(),(int)result.length());
                send.flush();
                send.endPacket();
            }
        }
    }
}


void FallbackNameServer::close() {
    closed = true;
    listen.interrupt();
}



#else

int FallbackNameServerDummySymbol = 42;

#endif // YARP_HAS_ACE






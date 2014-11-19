// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <yarp/conf/system.h>

#include <yarp/os/impl/DgramTwoWayStream.h>

#include <yarp/os/impl/Logger.h>
#include <yarp/os/Time.h>
#include <yarp/os/NetType.h>

#ifdef YARP_HAS_ACE
#  include <ace/SOCK_Dgram_Mcast.h>
#  include <ace/SOCK_Dgram.h>
#  include <ace/Handle_Set.h>
#  include <ace/Log_Msg.h>
#  include <ace/INET_Addr.h>
#  include <ace/ACE.h>
#  include <ace/OS_NS_string.h>
#  include <ace/OS_Memory.h>
#  include <ace/OS_NS_sys_select.h>
#  include <ace/os_include/net/os_if.h>
#else
#  include <arpa/inet.h>
#  include <netinet/in.h>
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <unistd.h>
#endif

#include <yarp/os/Time.h>

using namespace yarp::os::impl;
using namespace yarp::os;

#define CRC_SIZE 8
#define READ_SIZE (120000-CRC_SIZE)
#define WRITE_SIZE (60000-CRC_SIZE)


static bool checkCrc(char *buf, YARP_SSIZE_T length, YARP_SSIZE_T crcLength, int pct,
                     int *store_altPct = NULL) {
    NetInt32 alt =
        (NetInt32)NetType::getCrc(buf+crcLength,(length>crcLength)?(length-crcLength):0);
    Bytes b(buf,4);
    Bytes b2(buf+4,4);
    NetInt32 curr = NetType::netInt(b);
    int altPct = NetType::netInt(b2);
    bool ok = (alt == curr && pct==altPct);
    if (!ok) {
        if (alt!=curr) {
            yDebugNoFw("crc mismatch");
        }
        if (pct!=altPct) {
            yDebugNoFw("packet code broken");
        }
    }
    if (store_altPct!=NULL) {
        *store_altPct = altPct;
    }

    return ok;
}


static void addCrc(char *buf, YARP_SSIZE_T length, YARP_SSIZE_T crcLength, int pct) {
    NetInt32 alt =
        (NetInt32)NetType::getCrc(buf+crcLength,
                                           (length>crcLength)?(length-crcLength):0);
    Bytes b(buf,4);
    Bytes b2(buf+4,4);
    NetType::netInt((NetInt32)alt,b);
    NetType::netInt((NetInt32)pct,b2);
}


bool DgramTwoWayStream::open(const Contact& remote) {
#ifdef YARP_HAS_ACE
    ACE_INET_Addr anywhere((u_short)0, (ACE_UINT32)INADDR_ANY);
    Contact local(anywhere.get_host_addr(),
                  anywhere.get_port_number());
#else
    Contact local("localhost",-1);
#endif
    return open(local,remote);
}

bool DgramTwoWayStream::open(const Contact& local, const Contact& remote) {
    localAddress = local;
    remoteAddress = remote;

#ifdef YARP_HAS_ACE
    localHandle = ACE_INET_Addr((u_short)(localAddress.getPort()),(ACE_UINT32)INADDR_ANY);
    if (remote.isValid()) {
        remoteHandle.set(remoteAddress.getPort(),remoteAddress.getHost().c_str());
    }
    dgram = new ACE_SOCK_Dgram;
    yAssert(dgram!=NULL);

    int result = dgram->open(localHandle,
                             ACE_PROTOCOL_FAMILY_INET,
                             0,
                             1);
#else
    dgram = NULL;
    dgram_sockfd = -1;

    int s = -1;
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) {
        exit(1);
    }
    struct sockaddr_in dgram_sin;
    memset((char *) &dgram_sin, 0, sizeof(dgram_sin));
    dgram_sin.sin_family = AF_INET;
    dgram_sin.sin_addr.s_addr = htonl(INADDR_ANY);
    dgram_sin.sin_port = htons(remote.getPort());
    if (local.isValid()) {
        if (inet_aton(remote.getHost().c_str(), &dgram_sin.sin_addr)==0) {
            YARP_ERROR(Logger::get(), "could not set up udp client\n");
            exit(1);
        }
        if (connect(s, (struct sockaddr *)&dgram_sin, sizeof(dgram_sin))==-1) {
            YARP_ERROR(Logger::get(), "could not connect udp client\n");
            exit(1);
        }
    } else {
        if (bind(s, (struct sockaddr *)&dgram_sin, sizeof(dgram_sin))==-1) {
            YARP_ERROR(Logger::get(), "could not create udp server\n");
            exit(1);
        }
    }
    dgram_sockfd = s;
    dgram = this;
    int result = -1;
    int local_port = -1;

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    if (getsockname(dgram_sockfd, (struct sockaddr *)&sin, &len) == 0 &&
        sin.sin_family == AF_INET) {
        result = 0;
        local_port = ntohs(sin.sin_port);
    }
#endif

    if (result!=0) {
        yErrorNoFw("could not open datagram socket");
        return false;
    }

    configureSystemBuffers();

#ifdef YARP_HAS_ACE
    dgram->get_local_addr(localHandle);
    yDebugNoFw("starting DGRAM entity on port number %d", localHandle.get_port_number());
    localAddress = Contact("127.0.0.1",
                           localHandle.get_port_number());
#else
    localAddress = Contact("127.0.0.1",local_port);
#endif

    yDebugNoFw("Update: DGRAM from %s, to %s",
               localAddress.toURI().c_str(),
               remote.toURI().c_str());

    allocate();

    return true;
}

void DgramTwoWayStream::allocate(int readSize, int writeSize) {
    int _read_size = READ_SIZE+CRC_SIZE;
    int _write_size = WRITE_SIZE+CRC_SIZE;

    ConstString _env_dgram = NetworkBase::getEnvironment("YARP_DGRAM_SIZE");
    ConstString _env_mode = "";
    if (multiMode) {
        _env_mode = NetworkBase::getEnvironment("YARP_MCAST_SIZE");
    } else {
        _env_mode = NetworkBase::getEnvironment("YARP_UDP_SIZE");
    }
    if ( _env_mode!="") {
        _env_dgram = _env_mode;
    }
    if (_env_dgram!="") {
        int sz = NetType::toInt(_env_dgram);
        if (sz!=0) {
            _read_size = _write_size = sz;
        }
        yInfoNoFw("Datagram packet size set to %d", _read_size);
    }
    if (readSize!=0) {
        _read_size = readSize;
        yInfoNoFw("Datagram read size reset to %d", _read_size);
    }
    if (writeSize!=0) {
        _write_size = writeSize;
        yInfoNoFw("Datagram write size reset to %d", _write_size);
    }
    readBuffer.allocate(_read_size);
    writeBuffer.allocate(_write_size);
    readAt = 0;
    readAvail = 0;
    writeAvail = CRC_SIZE;
    //happy = true;
    pct = 0;
}


void DgramTwoWayStream::configureSystemBuffers() {
    // ask for more buffer space for udp/mcast

    ConstString _dgram_buffer_size = NetworkBase::getEnvironment("YARP_DGRAM_BUFFER_SIZE");

     int window_size_desired = 600000;

    if (_dgram_buffer_size!="")
        window_size_desired = NetType::toInt(_dgram_buffer_size);

    int window_size = window_size_desired;
#ifdef YARP_HAS_ACE
    int result = dgram->set_option(SOL_SOCKET, SO_RCVBUF,
                                   (char *) &window_size, sizeof(window_size));
#else
    int result = setsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF,
                            (void*) &window_size, sizeof(window_size));
#endif

    window_size = 0;
#ifdef YARP_HAS_ACE
    int len = 4;
    int result2 = dgram->get_option(SOL_SOCKET, SO_RCVBUF,
                                    (char *) &window_size, &len);
#else
    socklen_t len = 4;
    int result2 = getsockopt(dgram_sockfd, SOL_SOCKET, SO_RCVBUF,
                             (void *) &window_size, &len);
#endif
    if (result!=0||result2!=0||window_size<window_size_desired) {
        // give a warning if we get CRC problems
        bufferAlertNeeded = true;
        bufferAlerted = false;
    }
    yDebugNoFw("Warning: buffer size set to %d, you requested %d", window_size, window_size_desired);
}


#ifdef YARP_HAS_ACE
int DgramTwoWayStream::restrictMcast(ACE_SOCK_Dgram_Mcast * dmcast,
                                     const Contact& group,
                                     const Contact& ipLocal,
                                     bool add) {
    restrictInterfaceIp = ipLocal;

    yInfoNoFw("multicast connection %s on network interface for  %s", group.getHost().c_str(), ipLocal.getHost().c_str());
    int result = -1;
    // There's some major damage in ACE mcast interfaces.
    // Most require interface names, yet provide no way to query
    // these names - and in the end, convert to IP addresses.
    // Here we try to do an end run around ACE.

    // based on: ACE_SOCK_Dgram::set_nic

    ip_mreq multicast_address;
    ACE_INET_Addr group_addr(group.getPort(),
                             group.getHost().c_str());
    ACE_INET_Addr interface_addr(ipLocal.getPort(),
                                 ipLocal.getHost().c_str());
    multicast_address.imr_interface.s_addr =
        htonl (interface_addr.get_ip_address ());
    multicast_address.imr_multiaddr.s_addr =
        htonl (group_addr.get_ip_address ());

    if (add) {
        yDebugNoFw("Trying to correct mcast membership...");
        result =
            ((ACE_SOCK*)dmcast)->set_option (IPPROTO_IP,
                                             IP_ADD_MEMBERSHIP,
                                             &multicast_address,
                                             sizeof (struct ip_mreq));
    } else {
        yDebugNoFw("Trying to correct mcast output...");
        result =
            ((ACE_SOCK*)dmcast)->set_option (IPPROTO_IP,
                                             IP_MULTICAST_IF,
                                             &multicast_address.imr_interface.s_addr,
                                             sizeof (struct in_addr));

    }
    if (result!=0) {
        int num = errno;
        yDebugNoFw("mcast result: %d", num);
        if (num==98) {
            // our membership is already correct / Address already in use
            result = 0;
        }
        result = 0; // in fact, best to proceed for Windows.
    }

    return result;
}
#endif


bool DgramTwoWayStream::openMcast(const Contact& group,
                                  const Contact& ipLocal) {
#ifdef YARP_HAS_ACE
    multiMode = true;

    localAddress = ipLocal;
    localHandle = ACE_INET_Addr((u_short)(localAddress.getPort()),
                                (ACE_UINT32)INADDR_ANY);

    ACE_SOCK_Dgram_Mcast *dmcast = new ACE_SOCK_Dgram_Mcast;
    dgram = dmcast;
    mgram = dmcast;
    yAssert(dgram!=NULL);

    int result = -1;
    ACE_INET_Addr addr(group.getPort(),group.getHost().c_str());
    if (ipLocal.isValid()) {
        result = dmcast->open(addr,NULL,1);
        if (result==0) {
            result = restrictMcast(dmcast,group,ipLocal,false);
        }
    } else {
        result = dmcast->open(addr,NULL,1);
    }

    if (result!=0) {
        yErrorNoFw("could not open multicast datagram socket");
        return false;
    }

    configureSystemBuffers();

    remoteAddress = group;
    localHandle.set(localAddress.getPort(),localAddress.getHost().c_str());
    remoteHandle.set(remoteAddress.getPort(),remoteAddress.getHost().c_str());

    allocate();

    return true;
#else
    return false;
#endif
}


bool DgramTwoWayStream::join(const Contact& group, bool sender,
                             const Contact& ipLocal) {
#ifdef YARP_HAS_ACE
    yDebugNoFw("subscribing to mcast address %s for %s",
               group.toURI().c_str(),
               sender ? "writing" : "reading");

    multiMode = true;

    if (sender) {
        if (ipLocal.isValid()) {
            return openMcast(group,ipLocal);
        } else {
            // just use udp as normal
            return open(group);
        }
        //return;
    }

    ACE_SOCK_Dgram_Mcast *dmcast = new ACE_SOCK_Dgram_Mcast;

    //possible flags: ((ACE_SOCK_Dgram_Mcast::options)(ACE_SOCK_Dgram_Mcast::OPT_NULLIFACE_ALL | ACE_SOCK_Dgram_Mcast::OPT_BINDADDR_YES));

    dgram = dmcast;
    mgram = dmcast;
    yAssert(dgram!=NULL);

    ACE_INET_Addr addr(group.getPort(),group.getHost().c_str());

    int result = -1;
    if (ipLocal.isValid()) {
        result = 0;
        result = dmcast->join(addr,1);

        if (result==0) {
            result = restrictMcast(dmcast,group,ipLocal,true);
        }
    } else {
        result = dmcast->join(addr,1);
    }

    configureSystemBuffers();

    if (result!=0) {
        yErrorNoFw("cannot connect to multi-cast address");
        happy = false;
        return false;
    }
    localAddress = group;
    remoteAddress = group;
    localHandle.set(localAddress.getPort(),localAddress.getHost().c_str());
    remoteHandle.set(remoteAddress.getPort(),remoteAddress.getHost().c_str());

    allocate();
    return true;
#else
    return false;
#endif
}

DgramTwoWayStream::~DgramTwoWayStream() {
    closeMain();
}

void DgramTwoWayStream::interrupt() {
    bool act = false;
    mutex.wait();
    if ((!closed) && (!interrupting) && happy) {
        act = true;
        interrupting = true;
        closed = true;
    }
    mutex.post();

    if (act) {
        if (reader) {
            int ct = 3;
            while (happy && ct>0) {
                ct--;
                DgramTwoWayStream tmp;
                if (mgram) {
                    yDebugNoFw("* mcast interrupt, interface %s", restrictInterfaceIp.toString().c_str());
                    tmp.join(localAddress,true,
                             restrictInterfaceIp);
                } else {
                    yDebugNoFw("* dgram interrupt");
                    tmp.open(Contact(localAddress.getHost(),0),
                             localAddress);
                }
                yDebugNoFw("* interrupt state %s %s %s",
                           interrupting ? "true" : "false",
                           closed ? "true" : "false",
                           happy ? "true" : "false");
                ManagedBytes empty(10);
                for (size_t i=0; i<empty.length(); i++) {
                    empty.get()[i] = 0;
                }

                // don't want this message getting into a valid packet
                tmp.pct = -1;

                tmp.write(empty.bytes());
                tmp.flush();
                tmp.close();
                if (happy) {
                    yarp::os::Time::delay(0.25);
                }
            }
            yDebugNoFw("dgram interrupt done");
        }
        mutex.wait();
        interrupting = false;
        mutex.post();
    } else {
        // wait for interruption to be done
        if (interrupting) {
            while (interrupting) {
                yDebugNoFw("waiting for dgram interrupt to be finished...");
                yarp::os::Time::delay(0.1);
            }
        }
    }

}

void DgramTwoWayStream::closeMain() {
    if (dgram!=NULL) {
        //printf("Dgram closing, interrupt state %d\n", interrupting);
        interrupt();
        mutex.wait();
        closed = true;
        happy = false;
        //printf("Dgram closinger, interrupt state %d\n", interrupting);
        mutex.post();
        while (interrupting) {
            happy = false;
            yarp::os::Time::delay(0.1);
        }
        mutex.wait();
        if (dgram!=NULL) {
#ifdef YARP_HAS_ACE
            dgram->close();
            delete dgram;
#else
            if (dgram_sockfd>=0) {
                ::close(dgram_sockfd);
            }
            dgram_sockfd = -1;
#endif
            dgram = NULL;
            mgram = NULL;
        }
        happy = false;
        mutex.post();
    }
    happy = false;
}

YARP_SSIZE_T DgramTwoWayStream::read(const Bytes& b) {
    reader = true;
    bool done = false;

    while (!done) {

        if (closed) {
            happy = false;
            return -1;
        }

        // if nothing is available, try to grab stuff
        if (readAvail==0) {
            readAt = 0;


            //yAssert(dgram!=NULL);
            //yDebugNoFw("DGRAM Waiting for something!");
            YARP_SSIZE_T result = -1;
#ifdef YARP_HAS_ACE
            if (mgram && restrictInterfaceIp.isValid()) {
                /*
                printf("Consider remote mcast\n");
                printf("What we know:\n");
                printf("  %s\n",restrictInterfaceIp.toString().c_str());
                printf("  %s\n",localAddress.toString().c_str());
                printf("  %s\n",remoteAddress.toString().c_str());
                */
                ACE_INET_Addr iface(restrictInterfaceIp.getPort(),
                                    restrictInterfaceIp.getHost().c_str());
                ACE_INET_Addr dummy((u_short)0, (ACE_UINT32)INADDR_ANY);
                result =
                    dgram->recv(readBuffer.get(),readBuffer.length(),dummy);
                yDebugNoFw("MCAST Got %zd bytes", result);

            } else 
#endif
                if (dgram!=NULL) {
                yAssert(dgram!=NULL);
#ifdef YARP_HAS_ACE
                ACE_INET_Addr dummy((u_short)0, (ACE_UINT32)INADDR_ANY);
                //yDebugNoFw("DGRAM Waiting for something!");
                result =
                    dgram->recv(readBuffer.get(),readBuffer.length(),dummy);
#else
                result = recv(dgram_sockfd,readBuffer.get(),readBuffer.length(),0);
#endif
                yDebugNoFw("DGRAM Got %zd bytes", result);
            } else {
                onMonitorInput();
                //printf("Monitored input of %d bytes\n", monitor.length());
                if (monitor.length()>readBuffer.length()) {
                    printf("Too big!\n");
                    exit(1);
                }
                memcpy(readBuffer.get(),monitor.get(),monitor.length());
                result = monitor.length();
            }


            /*
              // this message isn't needed anymore
            if (result>WRITE_SIZE*1.25) {
                yErrorNoFw("Got big datagram: %d bytes", result);
            }
            */
            if (closed||(result<0)) {
                happy = false;
                return -1;
            }
            readAvail = result;

            // deal with CRC
            int altPct = 0;
            bool crcOk = checkCrc(readBuffer.get(),readAvail,CRC_SIZE,pct,
                                  &altPct);
            if (altPct!=-1) {
                pct++;
                if (!crcOk) {
                    if (bufferAlertNeeded&&!bufferAlerted) {
                        yErrorNoFw("*** Multicast/UDP packet dropped - checksum error ***");
                        yInfoNoFw("The UDP/MCAST system buffer limit on your system is low.");
                        yInfoNoFw("You may get packet loss under heavy conditions.");
#ifdef __linux__
                        yInfoNoFw("To change the buffer limit on linux: sysctl -w net.core.rmem_max=8388608");
                        yInfoNoFw("(Might be something like: sudo /sbin/sysctl -w net.core.rmem_max=8388608)");
#else
                        yInfoNoFw("To change the limit use: systcl for Linux/FreeBSD, ndd for Solaris, no for AIX");
#endif
                        bufferAlerted = true;
                    } else {
                        errCount++;
                        double now = Time::now();
                        if (now-lastReportTime>1) {
                            yErrorNoFw("*** %d datagram packet(s) dropped - checksum error ***", errCount);
                            lastReportTime = now;
                            errCount = 0;
                        }
                    }
                    reset();
                    return -1;
                } else {
                    readAt += CRC_SIZE;
                    readAvail -= CRC_SIZE;
                }
                done = true;
            } else {
                readAvail = 0;
            }
        }

        // if stuff is available, take it
        if (readAvail>0) {
            size_t take = readAvail;
            if (take>b.length()) {
                take = b.length();
            }
            ACE_OS::memcpy(b.get(),readBuffer.get()+readAt,take);
            readAt += take;
            readAvail -= take;
            return take;
        }
    }

    return 0;
}

void DgramTwoWayStream::write(const Bytes& b) {
    //yDebugNoFw("DGRAM prep writing");
    //ACE_OS::printf("DGRAM write %d bytes\n",b.length());

    if (reader) {
        return;
    }
    if (writeBuffer.get()==NULL) {
        return;
    }

    Bytes local = b;
    while (local.length()>0) {
        //yDebugNoFw("DGRAM prep writing");
        YARP_SSIZE_T rem = local.length();
        YARP_SSIZE_T space = writeBuffer.length()-writeAvail;
        bool shouldFlush = false;
        if (rem>=space) {
            rem = space;
            shouldFlush = true;
        }
        memcpy(writeBuffer.get()+writeAvail, local.get(), rem);
        writeAvail+=rem;
        local = Bytes(local.get()+rem,local.length()-rem);
        if (shouldFlush) {
            flush();
        }
    }
}


void DgramTwoWayStream::flush() {
    if (writeBuffer.get()==NULL) {
        return;
    }

    // should set CRC
    if (writeAvail<=CRC_SIZE) {
        return;
    }
    addCrc(writeBuffer.get(),writeAvail,CRC_SIZE,pct);
    pct++;

    while (writeAvail>0) {
        YARP_SSIZE_T writeAt = 0;
        //yAssert(dgram!=NULL);
        YARP_SSIZE_T len = 0;

#ifdef YARP_HAS_ACE
        if (mgram!=NULL) {
            len = mgram->send(writeBuffer.get()+writeAt,writeAvail-writeAt);
            yDebugNoFw("MCAST - wrote %zd bytes", len);
        } else 
#endif
            if (dgram!=NULL) {
#ifdef YARP_HAS_ACE
            len = dgram->send(writeBuffer.get()+writeAt,writeAvail-writeAt,
                              remoteHandle);
#else
            len = send(dgram_sockfd,writeBuffer.get()+writeAt,
                       writeAvail-writeAt,0);
#endif
            yDebugNoFw("DGRAM - wrote %zd bytes to %s", len, remoteAddress.toString().c_str());
        } else {
            Bytes b(writeBuffer.get()+writeAt,writeAvail-writeAt);
            monitor = ManagedBytes(b,false);
            monitor.copy();
            //printf("Monitored output of %d bytes\n", monitor.length());
            len = monitor.length();
            onMonitorOutput();
        }
        //if (len>WRITE_SIZE*0.75) {
        if (len>writeBuffer.length()*0.75) {
            yDebugNoFw("long dgrams might need a little time");

            // Under heavy loads, packets could get dropped
            // 640x480x3 images correspond to about 15 datagrams
            // so there's not much time possible between them
            // looked at iperf, it just does a busy-waiting delay
            // there's an implementation below, but commented out -
            // better solution was to increase recv buffer size

            double first = yarp::os::Time::now();
            double now = first;
            int ct = 0;
            do {
                //printf("Busy wait... %d\n", ct);
                yarp::os::Time::delay(0);
                now = yarp::os::Time::now();
                ct++;
            } while (now-first<0.001);
        }

        if (len<0) {
            happy = false;
            yDebugNoFw("DGRAM failed to write");
            return;
        }
        writeAt += len;
        writeAvail -= len;

        if (writeAvail!=0) {
            // well, we have a problem
            // checksums will cause dumping
            yDebugNoFw("dgram/mcast send behaving badly");
        }
    }
    // finally: writeAvail should be 0

    // make space for CRC
    writeAvail = CRC_SIZE;
}


bool DgramTwoWayStream::isOk() {
    return happy;
}


void DgramTwoWayStream::reset() {
    readAt = 0;
    readAvail = 0;
    writeAvail = CRC_SIZE;
    pct = 0;
}


void DgramTwoWayStream::beginPacket() {
    //yErrorNoFw("Packet begins: %s", reader?"reader":"writer");
    pct = 0;
}

void DgramTwoWayStream::endPacket() {
    //yErrorNoFw("Packet ends: %s", reader?"reader":"writer");
    if (!reader) {
        pct = 0;
    }
}

Bytes DgramTwoWayStream::getMonitor() {
    return monitor.bytes();
}


void DgramTwoWayStream::removeMonitor() {
    monitor.clear();
}

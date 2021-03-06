/*
* Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include "Rangefinder2DClient.h"
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

/*! \file Rangefinder2DClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


inline void  Rangefinder2DInputPortProcessor::resetStat()
{
    mutex.wait();
    count=0;
    deltaT=0;
    deltaTMax=0;
    deltaTMin=1e22;
    now=Time::now();
    prev=now;
    mutex.post();
}

Rangefinder2DInputPortProcessor::Rangefinder2DInputPortProcessor()
{
    state = IRangefinder2D::DEVICE_GENERAL_ERROR;
    resetStat();
}

void Rangefinder2DInputPortProcessor::onRead(yarp::os::Bottle &b)
{
    now=Time::now();
    mutex.wait();

    if (count>0)
    {
        double tmpDT=now-prev;
        deltaT+=tmpDT;
        if (tmpDT>deltaTMax)
            deltaTMax=tmpDT;
        if (tmpDT<deltaTMin)
            deltaTMin=tmpDT;

        //compare network time
        if (tmpDT*1000<LASER_TIMEOUT)
        {
            state = b.get(1).asInt();
        }
        else
        {
            state = IRangefinder2D::DEVICE_TIMEOUT;
        }
    }

    prev=now;
    count++;

    lastBottle=b;
    Stamp newStamp;
    getEnvelope(newStamp);

    //initialialization (first received data)
    if (lastStamp.isValid()==false)
    {
        lastStamp = newStamp;
    }

    //now compare timestamps
    if ((1000*(newStamp.getTime()-lastStamp.getTime()))<LASER_TIMEOUT)
    {
        state = b.get(1).asInt();
    }
    else
    {
        state = IRangefinder2D::DEVICE_TIMEOUT;
    }
    lastStamp = newStamp;

    mutex.post();
}

inline int Rangefinder2DInputPortProcessor::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    mutex.wait();
    int ret=state;
    if (ret != IRangefinder2D::DEVICE_GENERAL_ERROR)
    {
        data=lastBottle;
        stmp = lastStamp;
    }
    mutex.post();

    return ret;
}

bool Rangefinder2DInputPortProcessor::getData(yarp::sig::Vector &ranges)
{
    mutex.wait();
    if (lastBottle.size()==0) { mutex.post(); return false; }
    unsigned int size = lastBottle.get(0).asList()->size();
    ranges.resize(size);
    for (unsigned int i = 0; i < size; i++)
        ranges[i] = lastBottle.get(0).asList()->get(i).asDouble();
    mutex.post();
    return true;
}

yarp::dev::IRangefinder2D::Device_status Rangefinder2DInputPortProcessor::getStatus()
{
    mutex.wait();
    yarp::dev::IRangefinder2D::Device_status status = (yarp::dev::IRangefinder2D::Device_status) lastBottle.get(3).asInt();
    mutex.post();
    return status;
}

inline int Rangefinder2DInputPortProcessor::getIterations()
{
    mutex.wait();
    int ret=count;
    mutex.post();
    return ret;
}

// time is in ms
void Rangefinder2DInputPortProcessor::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    mutex.wait();
    ite=count;
    min=deltaTMin*1000;
    max=deltaTMax*1000;
    if (count<1)
    {
        av=0;
    }
    else
    {
        av=deltaT/count;
    }
    av=av*1000;
    mutex.post();
}

bool yarp::dev::Rangefinder2DClient::open(yarp::os::Searchable &config)
{
    local.clear();
    remote.clear();

    local  = config.find("local").asString().c_str();
    remote = config.find("remote").asString().c_str();

    if (local=="")
    {
        yError("Rangefinder2DClient::open() error you have to provide valid local name");
        return false;
    }
    if (remote=="")
    {
        yError("Rangefinder2DClient::open() error you have to provide valid remote name");
        return false;
    }

    if (config.check("period"))
    {
        _rate = config.find("period").asInt();
    }
    else
    {
        yError("Rangefinder2DClient::open() missing period parameter");
        return false;
    }

    ConstString local_rpc = local;
    local_rpc += "/rpc:o";
    ConstString remote_rpc = remote;
    remote_rpc += "/rpc:i";

    if (!inputPort.open(local.c_str()))
    {
        yError("Rangefinder2DClient::open() error could not open port %s, check network\n",local.c_str());
        return false;
    }
    inputPort.useCallback();

    if (!rpcPort.open(local_rpc.c_str()))
    {
        yError("Rangefinder2DClient::open() error could not open rpc port %s, check network\n", local_rpc.c_str());
        return false;
    }

    bool ok=Network::connect(remote.c_str(), local.c_str(), "udp");
    if (!ok)
    {
        yError("Rangefinder2DClient::open() error could not connect to %s\n", remote.c_str());
        return false;
    }

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        yError("Rangefinder2DClient::open() error could not connect to %s\n", remote_rpc.c_str());
       return false;
    }

    return true;
}

bool yarp::dev::Rangefinder2DClient::close()
{
    rpcPort.close();
    inputPort.close();
    return true;
}

bool yarp::dev::Rangefinder2DClient::getMeasurementData(yarp::sig::Vector &out)
{
    inputPort.getData(out);
    return true;
}

bool yarp::dev::Rangefinder2DClient::getDistanceRange(double& min, double& max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_DISTANCE_RANGE);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        min = response.get(2).asDouble();
        max = response.get(3).asDouble();
        return true;
    }
    return false;
}

bool yarp::dev::Rangefinder2DClient::setDistanceRange(double min, double max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_DISTANCE_RANGE);
    cmd.addDouble(min);
    cmd.addDouble(max);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool yarp::dev::Rangefinder2DClient::getScanLimits(double& min, double& max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_RANGE);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        min = response.get(2).asDouble();
        max = response.get(3).asDouble();
        return true;
    }
    return false;
}

bool yarp::dev::Rangefinder2DClient::setScanLimits(double min, double max)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_RANGE);
    cmd.addDouble(min);
    cmd.addDouble(max);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool yarp::dev::Rangefinder2DClient::getHorizontalResolution(double& step)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_STEP);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        step = response.get(2).asDouble();
        return true;
    }
    return false;
}

bool yarp::dev::Rangefinder2DClient::setHorizontalResolution(double step)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_ANGULAR_STEP);
    cmd.addDouble(step);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool yarp::dev::Rangefinder2DClient::getScanRate(double& rate)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_SCAN_RATE);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response) != false)
    {
        rate = response.get(2).asDouble();
        return true;
    }
    return false;
}

bool yarp::dev::Rangefinder2DClient::setScanRate(double rate)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_SET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_LASER_SCAN_RATE);
    cmd.addDouble(rate);
    bool ok = rpcPort.write(cmd, response);
    return (CHECK_FAIL(ok, response));
}

bool yarp::dev::Rangefinder2DClient::getDeviceStatus(Device_status &status)
{
    status = inputPort.getStatus();
    return true;
}

bool yarp::dev::Rangefinder2DClient::getDeviceInfo(yarp::os::ConstString &device_info)
{
    Bottle cmd, response;
    cmd.addVocab(VOCAB_GET);
    cmd.addVocab(VOCAB_ILASER2D);
    cmd.addVocab(VOCAB_DEVICE_INFO);
    bool ok = rpcPort.write(cmd, response);
    if (CHECK_FAIL(ok, response)!=false)
    {
        device_info = response.get(2).asString();
        return true;
    }
    return false;
}

Stamp yarp::dev::Rangefinder2DClient::getLastInputStamp()
{
    return lastTs;
}

yarp::dev::DriverCreator *createRangefinder2DClient() {
    return new DriverCreatorOf<Rangefinder2DClient>("Rangefinder2DClient",
        "",
        "Rangefinder2DClient");
}

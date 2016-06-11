/*
* Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
* Author: Marco Randazzo <marco.randazzo@iit.it>
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*
*/

#include <TransformClient.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LockGuard.h>

/*! \file TransformClient.cpp */

using namespace yarp::dev;
using namespace yarp::os;
using namespace yarp::sig;


inline void Transforms_client_storage::resetStat()
{

}

void Transforms_client_storage::onRead(yarp::os::Bottle &b)
{
    m_now = Time::now();
    LockGuard guard(m_mutex);

    if (m_count>0)
    {
        double tmpDT = m_now - m_prev;
        m_deltaT += tmpDT;
        if (tmpDT>m_deltaTMax)
            m_deltaTMax = tmpDT;
        if (tmpDT<m_deltaTMin)
            m_deltaTMin = tmpDT;

        //compare network time
        /*if (tmpDT*1000<TRANSFORM_TIMEOUT)
        {
            state = b.get(5).asInt();
        }
        else
        {
            state = TRANSFORM_TIMEOUT;
        }*/
    }

    m_prev = m_now;
    m_count++;

    m_lastBottle = b;
    Stamp newStamp;
    getEnvelope(newStamp);

    //initialialization (first received data)
    if (m_lastStamp.isValid() == false)
    {
        m_lastStamp = newStamp;
    }

    //now compare timestamps
    if ((1000 * (newStamp.getTime() - m_lastStamp.getTime()))<TRANSFORM_TIMEOUT_MS)
    {
        m_state = ITransform::TRANSFORM_OK;

        m_transforms.clear();
        for (int i = 0; i < b.size(); i++)
        {
            Bottle* bt = b.get(i).asList();
            if (bt != 0)
            {
                Transform_t t;
                t.src_frame_id = bt->get(0).asString();
                t.dst_frame_id = bt->get(1).asString();
                t.translation.tX = bt->get(2).asDouble();
                t.translation.tY = bt->get(3).asDouble();
                t.translation.tZ = bt->get(4).asDouble();
                t.rotation.rX = bt->get(5).asDouble();
                t.rotation.rY = bt->get(6).asDouble();
                t.rotation.rZ = bt->get(7).asDouble();
                t.rotation.rW = bt->get(8).asDouble();
                m_transforms.push_back(t);
            }
        }
    }
    else
    {
        m_state = ITransform::TRANSFORM_TIMEOUT;
    }
    m_lastStamp = newStamp;
}

inline int Transforms_client_storage::getLast(yarp::os::Bottle &data, Stamp &stmp)
{
    LockGuard guard(m_mutex);

    int ret = m_state;
    if (ret != ITransform::TRANSFORM_GENERAL_ERROR)
    {
        data = m_lastBottle;
        stmp = m_lastStamp;
    }

    return ret;
}

inline int Transforms_client_storage::getIterations()
{
    LockGuard guard(m_mutex);
    int ret = m_count;
    return ret;
}

// time is in ms
void Transforms_client_storage::getEstFrequency(int &ite, double &av, double &min, double &max)
{
    LockGuard guard(m_mutex);
    ite=m_count;
    min=m_deltaTMin*1000;
    max=m_deltaTMax*1000;
    if (m_count<1)
    {
        av=0;
    }
    else
    {
        av=m_deltaT/m_count;
    }
    av=av*1000;
}

Transforms_client_storage::Transforms_client_storage(std::string local_streaming_name)
{
    m_count = 0;
    m_deltaT = 0;
    m_deltaTMax = 0;
    m_deltaTMin = 1e22;
    m_now = Time::now();
    m_prev = m_now;

    if (!this->open(local_streaming_name.c_str()))
    {
        yError("TransformClient::open() error could not open port %s, check network", local_streaming_name.c_str());
    }
    this->useCallback();
}

Transforms_client_storage::~Transforms_client_storage()
{
    this->interrupt();
    this->close();
}

bool yarp::dev::TransformClient::open(yarp::os::Searchable &config)
{
    m_local_name.clear();
    m_remote_name.clear();

    m_local_name  = config.find("local").asString().c_str();
    m_remote_name = config.find("remote").asString().c_str();

    if (m_local_name == "")
    {
        yError("TransformClient::open() error you have to provide valid local name");
        return false;
    }
    if (m_remote_name == "")
    {
        yError("TransformClient::open() error you have to provide valid remote name");
        return false;
    }

    if (config.check("period"))
    {
        m_period = config.find("period").asInt();
    }
    else
    {
        m_period = 10;
        yWarning("TransformClient: using default period of %d ms" , m_period);
    }

    ConstString local_rpc = m_local_name;
    local_rpc += "/rpc:o";
    ConstString remote_rpc = m_remote_name;
    remote_rpc += "/rpc:i";
    ConstString remote_streaming_name = m_remote_name;
    remote_streaming_name += ":o";
    ConstString local_streaming_name = m_local_name;
    local_streaming_name += ":i";

    if (!m_rpcPort.open(local_rpc.c_str()))
    {
        yError("TransformClient::open() error could not open rpc port %s, check network", local_rpc.c_str());
        return false;
    }

    m_transform_storage = new Transforms_client_storage(local_streaming_name);
    bool ok=Network::connect(remote_streaming_name.c_str(), local_streaming_name.c_str(), "udp");
    if (!ok)
    {
        yError("TransformClient::open() error could not connect to %s", remote_streaming_name.c_str());
        return false;
    }

    ok=Network::connect(local_rpc.c_str(), remote_rpc.c_str());
    if (!ok)
    {
        yError("TransformClient::open() error could not connect to %s", remote_rpc.c_str());
        return false;
    }

    return true;
}

bool yarp::dev::TransformClient::close()
{
    m_rpcPort.close();
    if (m_transform_storage == 0)
    {
        delete m_transform_storage;
        m_transform_storage = 0;
    }
    return true;
}

std::string yarp::dev::TransformClient::allFramesAsString()
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::canTransform(const std::string &target_frame, const std::string &source_frame, std::string *error_msg)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::clear()
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::frameExists(const std::string &frame_id)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        if (((*m_transform_storage)[i].src_frame_id) == frame_id) { return true; }
        if (((*m_transform_storage)[i].dst_frame_id) == frame_id) { return true; }
    }
    return false;
}

bool yarp::dev::TransformClient::getAllFrameIds(std::vector< std::string > &ids)
{
    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        bool found = false;
        for (size_t j = 0; j < ids.size(); j++)
        {
            if (((*m_transform_storage)[i].src_frame_id) == ids[j]) { found = true; break; }
        }
        if (found == false) ids.push_back((*m_transform_storage)[i].src_frame_id);
    }

    for (size_t i = 0; i < m_transform_storage->size(); i++)
    {
        bool found = false;
        for (size_t j = 0; j < ids.size(); j++)
        {
            if (((*m_transform_storage)[i].dst_frame_id) == ids[j]) { found = true; break; }
        }
        if (found == false) ids.push_back((*m_transform_storage)[i].dst_frame_id);
    }

    return true;
}

bool yarp::dev::TransformClient::getParent(const std::string &frame_id, std::string &parent_frame_id)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::getTransform(const std::string &target_frame_id, const std::string &source_frame_id, yarp::sig::Matrix &transform)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::setTransform(const std::string &target_frame_id, const std::string &source_frame_id, const yarp::sig::Matrix &transform)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::deleteTransform(const std::string &target_frame_id, const std::string &source_frame_id)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::transformPoint(const std::string &target_frame_id, const yarp::sig::Vector &input_point, yarp::sig::Vector &transformed_point)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::transformPose(const std::string &target_frame_id, const yarp::sig::Vector &input_pose, yarp::sig::Vector &transformed_pose)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::transformQuaternion(const std::string &target_frame_id, const yarp::sig::Vector &input_quaternion, yarp::sig::Vector &transformed_quaternion)
{
    yError() << "Not yet implemented"; return false;
}

bool yarp::dev::TransformClient::waitForTransform(const std::string &target_frame_id, const std::string &source_frame_id, const double &timeout)
{
    yError() << "Not yet implemented"; return false;
}

/*Stamp yarp::dev::TransformClient::getLastInputStamp()
{
    return m_lastTs;
}*/

yarp::dev::DriverCreator *createTransformClient() {
    return new DriverCreatorOf<TransformClient>("transformClient",
        "",
        "transformClient");
}

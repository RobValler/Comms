/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "comm_server.h"

#include "iprotocol_server.h"
#include "tcpip_server.h"
#include "posix_mq_server.h"

#include "iserialiser.h"
#include "proto_helper.h"
#include "basic.h"

#include "Logger.h"

#include <string>
#include <thread>

CCommServer::CCommServer(server_proto::EProtocolType protocol, server_proto::ESerialType serial)
{
    CLogger::GetInstance();

    switch(protocol)
    {
        case server_proto::EProtocolType::EPT_None:
            ///\ todo add something here
            break;
        case server_proto::EProtocolType::EPT_TCTPIP:
            m_pProtocolServer = std::make_shared<comms::tcpip::server::CTCPIPServer>();
            break;
        case server_proto::EProtocolType::EPT_POSIX_MQ:
            m_pProtocolServer = std::make_shared<comms::posix::server::CPOSIXMQServer>();
            break;
    }

    switch(serial)
    {
        case server_proto::ESerialType::EST_None:
            m_pSerialiser = std::make_shared<comms::serial::basic::CSerialiserBasic>();
            break;
        case server_proto::ESerialType::EST_PROTO:
            m_pSerialiser = std::make_shared<comms::serial::protobuf::CSerialiserProto>();
            break;
    }

//    t_create = std::thread(&CCommServer::createThread, this);
//    createThread();
}

CCommServer::~CCommServer()
{
    m_shutdownrequest = true;

    if(t_read.joinable())
        t_read.join();

    if(t_write.joinable())
        t_write.join();

}

bool CCommServer::connect(std::string server_address)
{
    return m_pProtocolServer->client_connect(server_address);
}

bool CCommServer::init()
{
    if(!m_pProtocolServer->channel_create("name"))
        return false;

    t_read = std::thread(&CCommServer::readThread, this);
    t_write = std::thread(&CCommServer::writeThread, this);

    return true;
}

bool CCommServer::read(void* message)
{
    if(true == m_shutdownrequest)
        return false;

    if(0U == m_read_queue.size())
    {
        CLOG(LOGLEV_RUN, "read queue is empty");
        return false;
    }

    // fetch from buffer
    m_readQueueProtect.lock();
    m_readcall_container = std::move(m_read_queue.front());
    m_read_queue.pop();
    m_readQueueProtect.unlock();

    // deserialise the input stream
    if(!m_pSerialiser->deserialise(m_readcall_container.payload, message, m_readcall_container.payload.size()))
    {
        CLOG(LOGLEV_RUN, "deserialise returned an error");
        return false;
    }

    return true;
}

bool CCommServer::write(void* message, int size)
{
    if(true == m_shutdownrequest)
        return false;

    // serialise the output stream
    int size_of_write_message = size;
    if(!m_pSerialiser->serialise(message, m_writecall_container.payload, size_of_write_message))
    {
        CLOG(LOGLEV_RUN, "serialiser returned an error");
        return false;
    }

    if(0U == m_writecall_container.payload.size())
    {
        CLOG(LOGLEV_RUN, "The write container is empty and so won't be written to the queue");
        return false;
    }

    // add to write queue
    m_writeQueueProtect.lock();
    m_write_queue.push(std::move(m_writecall_container));
    m_writeQueueProtect.unlock();

    // notify the write thread
    cv_writeThread.notify_one();

    return true;
}

int CCommServer::sizeOfReadBuffer()
{
    //return m_pProtocolServer->sizeOfReadBuffer();
    return 0;
}

void CCommServer::readThread()
{
    int size = 0;
    while(!m_shutdownrequest)
    {
        // fetch the intput stream - blocking call
        if(!m_pProtocolServer->recieve(m_readthread_container.payload, size))
        {
            CLOG(LOGLEV_RUN, "recieve returned an error");
            break;
        }

        // write to read buffer
        m_readQueueProtect.lock();
        m_read_queue.push(m_readthread_container);
        m_readQueueProtect.unlock();                
    }

    m_shutdownrequest = true;
}

void CCommServer::writeThread()
{
    while(!m_shutdownrequest)
    {
        // block the loop until new data comes
        std::unique_lock<std::mutex> thread_lock(cv_m);
        cv_writeThread.wait(thread_lock);

        m_writeQueueProtect.lock();
        //m_write_container = std::move(m_write_queue.front());
        m_writethread_container = m_write_queue.front();
        m_write_queue.pop();
        m_writeQueueProtect.unlock();

        if(0U == m_writethread_container.payload.size())
        {
            CLOG(LOGLEV_RUN, "write queue entry is empty");
            continue;
        }

        // transmit the output stream
        if(!m_pProtocolServer->transmit(m_writethread_container.payload.data(), m_writethread_container.payload.size()))
        {
            CLOG(LOGLEV_RUN, "transmition returned an error");
            break;
        }
    }

    m_shutdownrequest = true;
}

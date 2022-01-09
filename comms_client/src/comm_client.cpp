/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "comm_client.h"
#include "tcpip_client.h"
#include "posix_mq_client.h"

#include "iserialiser.h"
#include "proto_helper.h"
#include "basic.h"
#include "common.h"

#include <string>
#include <iostream>

#include "Logger.h"

CCommClient::CCommClient(client_proto::EProtocolType protocol, client_proto::ESerialType serial)
{
    CLogger::GetInstance();

    switch(protocol)
    {
    case client_proto::EProtocolType::EPT_None:
        ///\ todo add something here
        break;
    case client_proto::EProtocolType::EPT_TCTPIP:
        m_pProtocolClient = std::make_unique<comms::tcpip::client::CTCPIPClient>();
        break;
    case client_proto::EProtocolType::EPT_POSIX_MQ:
        m_pProtocolClient = std::make_unique<comms::posix::client::CPOSIXMQClient>();
        break;
    }

    switch(serial)
    {
        case client_proto::ESerialType::EST_None:
            m_pSerialiser = std::make_shared<comms::serial::basic::CSerialiserBasic>();
            break;
        case client_proto::ESerialType::EST_PROTO:
            m_pSerialiser = std::make_shared<comms::serial::protobuf::CSerialiserProto>();
            break;
    }
}

CCommClient::~CCommClient()
{
    m_pProtocolClient->client_disconnect();
    m_shutdownrequest = true;

    if(t_read.joinable())
        t_read.join();

    if(t_write.joinable())
        t_write.join();
}

bool CCommClient::connect(std::string server_address)
{
    if(!m_pProtocolClient->client_connect(server_address))
        return false;

    t_read = std::thread(&CCommClient::readThread, this);
    t_write = std::thread(&CCommClient::writeThread, this);

    return true;
}

bool CCommClient::read(void* message, int& size)
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

    size = m_readcall_container.payload.size();

    // deserialise the input stream
    if(!m_pSerialiser->deserialise(m_readcall_container.payload, message, m_readcall_container.payload.size()))
    {
        CLOG(LOGLEV_RUN, "deserialise returned an error");
        return false;
    }

    return true;
}

bool CCommClient::write(void* message, int size)
{
    if(true == m_shutdownrequest)
        return false;

    // serialise the output stream
    int size_of_write_message = size;
    client_proto::SReadBufferQ m_writecall_container{};
    if(!m_pSerialiser->serialise(message, m_writecall_container.payload, size_of_write_message))
    {
        CLOG(LOGLEV_RUN, "serialiser returned an error");
        return false;
    }

    // add to write queue
    m_writeQueueProtect.lock();
    m_write_queue.push(std::move(m_writecall_container));
    m_writeQueueProtect.unlock();

    return true;
}

int CCommClient::numOfMessages()
{    
//    m_writeQueueProtect.lock();
    int size = m_read_queue.size();
//    m_writeQueueProtect.unlock();
    return size;
}

void CCommClient::readThread()
{
    int size = 0;
    while(!m_shutdownrequest)
    {
        // fetch the intput stream
        if(!m_pProtocolClient->recieve(m_readthread_container.payload, size))
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
    CLOG(LOGLEV_RUN, "thread exited");
}

void CCommClient::writeThread()
{
    while(!m_shutdownrequest)
    {
        if(0U == m_write_queue.size())
        {
            continue;
        }

        m_writeQueueProtect.lock();
        m_writethread_container = std::move(m_write_queue.front());
        m_write_queue.pop();
        m_writeQueueProtect.unlock();

        // transmit the output stream
        if(!m_pProtocolClient->transmit(m_writethread_container.payload.data(), m_writethread_container.payload.size()))
        {
            CLOG(LOGLEV_RUN, "transmition returned an error");
        }
    }

    m_shutdownrequest = true;
    CLOG(LOGLEV_RUN, "thread exited");
}

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
}

bool CCommClient::connect(std::string server_address)
{
    return m_pProtocolClient->client_connect(server_address);
}

bool CCommClient::read(void* message)
{
    // fetch the intput stream
    if(!m_pProtocolClient->recieve(m_read_buffer, m_size_of_message))
    {
        CLOG(LOGLEV_RUN, "protocol recieve returned an error");
        return false;
    }

    // deserialise the input stream
    if(!m_pSerialiser->deserialise(m_read_buffer, m_size_of_message, message))
    {
        CLOG(LOGLEV_RUN, "deserialiser returned an error");
        return false;
    }

    return true;
}

bool CCommClient::write(void* message, int size)
{
    // serialise the output stream
    m_size_of_message = size;
    if(!m_pSerialiser->serialise(m_write_buffer, m_size_of_message, message))
    {
        CLOG(LOGLEV_RUN, "serialiser returned an error");
        return false;
    }

    // transmit the output stream
    if(!m_pProtocolClient->transmit(m_write_buffer.data(), m_size_of_message))
    {
        CLOG(LOGLEV_RUN, "transmit returned an error");
        return false;
    }

    return true;
}

int CCommClient::numOfMessages()
{
    int number = 0;



    return number;
}

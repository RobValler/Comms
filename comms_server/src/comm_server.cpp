/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
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

#include "Logger.h"

#include <string>
#include <vector>

CCommServer::CCommServer(server_proto::EProtocolType type)
{
    CLogger::GetInstance();

    switch(type)
    {
    case server_proto::ENone:

        break;
    case server_proto::ETCTPIP:
        m_pProtocolServer = std::make_shared<comms::tcpip::server::CTCPIPServer>();
        m_pSerialiser = std::make_shared<comms::serial::protobuf::CSerialiserHelper>();
        break;
    case server_proto::EPOSIX_MQ:
        m_pProtocolServer = std::make_shared<comms::posix::server::CPOSIXMQServer>();
        m_pSerialiser = std::make_shared<comms::serial::protobuf::CSerialiserHelper>();
        break;
    }
}

bool CCommServer::connect(std::string server_address)
{
    return m_pProtocolServer->client_connect(server_address);
}

bool CCommServer::read(void* message)
{
    // fetch the intput stream
    if(!m_pProtocolServer->recieve(m_read_buffer, m_size_of_message))
    {
        CLOG(LOGLEV_RUN, "protocol recieve returned an error");
        return false;
    }

    // deserialise the input stream
    if(!m_pSerialiser->deserialise(m_read_buffer, m_size_of_message, message))
    {
        CLOG(LOGLEV_RUN, "serialiser returned an error");
        return false;
    }

    return true;
}

bool CCommServer::write(void* message)
{
    // serialise the output stream
    if(!m_pSerialiser->serialise(m_write_buffer, m_size_of_message, message))
    {
        CLogger::Print(LOGLEV_RUN, "read.", " serialiser returned an error");
        return false;
    }

    // transmit the output stream
    if(!m_pProtocolServer->transmit(m_write_buffer.data(), m_size_of_message))
        return false;

    return true;
}

int CCommServer::sizeOfReadBuffer()
{
    return m_pProtocolServer->sizeOfReadBuffer();
}

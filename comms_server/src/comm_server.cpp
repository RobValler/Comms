/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
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

CCommServer::CCommServer(EProtocolType type)
{
    CLogger::GetInstance();

    switch(type)
    {
    case ENone:

        break;
    case ETCTPIP:
        m_pProtocolServer = std::make_shared<comms::tcpip::server::CTCPIPServer>();
        m_pSerialiser = std::make_shared<comms::serial::protobuf::CSerialiserHelper>();
        break;
    case EPOSIX_MQ:
        m_pProtocolServer = std::make_shared<comms::posix::server::CPOSIXMQServer>();
        m_pSerialiser = std::make_shared<comms::serial::protobuf::CSerialiserHelper>();
        break;
    }
}

bool CCommServer::read(void* message)
{
    int size_of_message;
    std::vector<char> buffer;

    // fetch the intput stream
    if(!m_pProtocolServer->recieve(buffer, size_of_message))
    {
        CLOG(LOGLEV_RUN, "protocol recieve returned an error");
        return false;
    }

    // deserialise the input stream
    if(!m_pSerialiser->deserialise(buffer, size_of_message, message))
    {
        CLOG(LOGLEV_RUN, "serialiser returned an error");
        return false;
    }

    return true;
}

bool CCommServer::write(void* message)
{
    int size_of_message = 0;
    std::vector<char> buffer(size_of_message);

    // serialise the output stream
    if(!m_pSerialiser->serialise(buffer, size_of_message, message))
    {
        CLogger::Print(LOGLEV_RUN, "read.", " serialiser returned an error");
        return false;
    }

    // transmit the output stream
    if(!m_pProtocolServer->transmit(&buffer[0], size_of_message))
        return false;

    return true;
}

int CCommServer::sizeOfReadBuffer()
{
    return m_pProtocolServer->sizeOfReadBuffer();
}

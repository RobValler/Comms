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
#include "iserialiser.h"
#include "proto_helper.h"

//#include "posix_mq_server.h"

#include "Logger.h"

#include <string>
#include <vector>
#include <google/protobuf/message.h>

enum EProtocolType : unsigned int
{
    ENone = 0,
    ETCTPIP,
    EPOSIX_MQ
};


CCommServer::CCommServer(EProtocolType type)
{
    CLogger::GetInstance();

    switch(type)
    {
    case ENone:

        break;
    case ETCTPIP:
        m_pProtocolServer = std::make_shared<comms::tcpip::server::CTCPIPServer>();
        m_pSerialiser = std::make_shared<comms::serial::protobuf::CSerialiser>();

        break;
    case EPOSIX_MQ:
        //m_pProtocolServer = std::make_shared<comms::posix::server::CPOSIXMQServer>();
        break;
    }
}

bool CCommServer::read(::google::protobuf::Message& message)
{
    int size_of_message;
    std::vector<char> buffer;

    // fetch the intput stream
    if(!m_pProtocolServer->recieve(buffer, size_of_message))
    {
        CLogger::Print(LOGLEV_RUN, "read.", " protocol recieve returned an error");
        return false;
    }

    // deserialise the input stream
    if(!m_pSerialiser->deserialise(buffer, size_of_message, &message))
    {
        CLogger::Print(LOGLEV_RUN, "read.", " serialiser returned an error");
        return false;
    }

    return true;
}

bool CCommServer::write(::google::protobuf::Message& message)
{
    int size_of_message = 0;
    std::vector<char> buffer(size_of_message);

    // serialise the output stream
    if(!m_pSerialiser->serialise(buffer, size_of_message, &message))
    {
        CLogger::Print(LOGLEV_RUN, "read.", " serialiser returned an error");
        return false;
    }

    // transmit the output stream
    if(!m_pProtocolServer->transmit(&buffer[0], size_of_message))
        return false;

    return true;
}

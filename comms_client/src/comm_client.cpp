/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "comm_client.h"
#include "tcpip_client.h"
#include "iserialiser.h"
#include "proto_helper.h"

//#include "posix_mq_client.h"
#include "common.h"

#include <string>
#include <iostream>

#include "Logger.h"

#include <google/protobuf/message.h>


CCommClient::CCommClient(EProtocolType type)
{
    CLogger::GetInstance();

    switch(type)
    {
    case ENone:

        break;
    case ETCTPIP:
        m_pProtocolClient = std::make_unique<comms::tcpip::client::CTCPIPClient>();
        m_pSerialiser = std::make_shared<comms::serial::protobuf::CSerialiserHelper>();
        break;
    case EPOSIX_MQ:
 //       m_pProtocolClient = std::make_unique<comms::posix::client::CPOSIXMQClient>();
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
bool CCommClient::read(::google::protobuf::Message& message)
{
    int size_of_message;
    std::vector<char> buffer;

    // fetch the intput stream
    if(!m_pProtocolClient->recieve(buffer, size_of_message))
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

bool CCommClient::write(::google::protobuf::Message& message)
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
    if(!m_pProtocolClient->transmit(&buffer[0], size_of_message))
        return false;

    return true;
}

int CCommClient::numOfMessages()
{
    int number = 0;



    return number;
}

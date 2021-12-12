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

// todo: make dynamic ... shared lib loader?
#include "tcpip_server.h"
//#include "posix_mq_server.h"

#include "Logger.h"

#include <string>
#include <vector>


#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

enum EProtocolType : unsigned int
{
    ENone = 0,
    ETCTPIP,
    EPOSIX_MQ
};


CCommServer::CCommServer(EProtocolType type)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    CLogger::GetInstance();

    switch(type)
    {
    case ENone:

        break;
    case ETCTPIP:
        m_pProtocolServer = std::make_shared<comms::tcpip::server::CTCPIPServer>();
        break;
    case EPOSIX_MQ:
        //m_pProtocolServer = std::make_shared<comms::posix::server::CPOSIXMQServer>();
        break;
    }
}

CCommServer::~CCommServer()
{
    google::protobuf::ShutdownProtobufLibrary();
}

bool CCommServer::read(::google::protobuf::Message& message)
{
    using namespace google::protobuf::io;

    int size_of_message;
    std::vector<char> buffer;

    //fetch data
    if(!m_pProtocolServer->recieve(buffer, size_of_message))
    {
        CLogger::Print(LOGLEV_RUN, "read.", " protocol recieve returned error");
        return false;
    }

    // convert from serialised char array to protobuf message class
    google::protobuf::io::ArrayInputStream ais(&buffer[0], size_of_message);
    CodedInputStream coded_input(&ais);
    std::uint32_t size = static_cast<std::uint32_t>(size_of_message);
    coded_input.ReadVarint32(&size);
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(size);
    message.ParseFromCodedStream(&coded_input);
    coded_input.ConsumedEntireMessage();
    coded_input.PopLimit(msgLimit);

    return true;
}

bool CCommServer::write(const ::google::protobuf::Message& message)
{
    using namespace google::protobuf::io;

    // serialise data
    std::uint32_t size = message.ByteSizeLong();
    size += CodedOutputStream::VarintSize32(size);
    std::vector<char> pkt(size);
    google::protobuf::io::ArrayOutputStream aos(&pkt[0], size);
    CodedOutputStream coded_output(&aos);
    coded_output.WriteVarint32(message.ByteSizeLong());

    if(!message.SerializeToCodedStream(&coded_output))
        return false;

    // send data
    if(!m_pProtocolServer->transmit(&pkt[0], size))
        return false;

    return true;
}

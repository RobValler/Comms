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
#include "posix_mq_server.h"

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
        m_pProtocolServer = std::make_shared<comms::posix::server::CPOSIXMQServer>();
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

    int siz;
    //char *pkt = nullptr;

    std::vector<char> tmp;

    //fetch data
    if(!m_pProtocolServer->recieve(tmp, siz)) {
        CLogger::Print(LOGLEV_RUN, "read.", " protocol recieve returned error");
        return false;
    }

    google::protobuf::io::ArrayInputStream ais(&tmp, siz);
    CodedInputStream coded_input(&ais);
    std::uint32_t s = static_cast<std::uint32_t>(siz);
    if(!coded_input.ReadVarint32(&s))
        return false;
    coded_input.ReadVarint32(&s);

    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
    if(!message.ParseFromCodedStream(&coded_input))
        return false;

    coded_input.PopLimit(msgLimit);

    return true;
}

bool CCommServer::write(const ::google::protobuf::Message& message)
{
    using namespace google::protobuf::io;
    std::vector<char> pkt;

    // serialise data
    std::uint32_t siz = message.ByteSizeLong();
    siz += CodedOutputStream::VarintSize32(siz);
    pkt.resize(siz);
    google::protobuf::io::ArrayOutputStream aos(&pkt[0], siz);
    std::unique_ptr<CodedOutputStream> coded_output = std::make_unique<CodedOutputStream>(&aos);
    coded_output->WriteVarint32(message.ByteSizeLong());
    if(!message.SerializeToCodedStream(coded_output.get()))
        return false;

    // send data
    if(!m_pProtocolServer->transmit(&pkt[0], siz))
        return false;

    return true;
}

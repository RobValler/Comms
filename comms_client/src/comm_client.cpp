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
#include "posix_mq_client.h"
#include "common.h"

#include <string>
#include <iostream>

#include "Logger.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

CCommClient::CCommClient(EProtocolType type)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    CLogger::GetInstance();

    switch(type)
    {
    case ENone:

        break;
    case ETCTPIP:
        m_pProtocolClient = std::make_shared<CTCPIPClient>();
        break;
    case EPOSIX_MQ:
        m_pProtocolClient = std::make_shared<comms::posix::client::CPOSIXMQClient>();
        break;
    }

}

CCommClient::~CCommClient()
{
    m_pProtocolClient->client_disconnect();
    google::protobuf::ShutdownProtobufLibrary();
}

bool CCommClient::connect(std::string server_address)
{
    return m_pProtocolClient->client_connect(server_address);
}

bool CCommClient::read(::google::protobuf::Message& message)
{
    using namespace google::protobuf::io;

    int siz;
    char *pkt = nullptr;

    //fetch data
    if(!m_pProtocolClient->recieve(&pkt, siz)) {
        CLOG(LOGLEV_RUN, "protocol recieve returned error");
        return false;
    }

    google::protobuf::io::ArrayInputStream ais(pkt, siz);
    CodedInputStream coded_input(&ais);
    std::uint32_t s = static_cast<std::uint32_t>(siz);
//    if(!coded_input.ReadVarint32(&s))
//        return false;
    coded_input.ReadVarint32(&s);

    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
    if(!message.ParseFromCodedStream(&coded_input))
        return false;

    coded_input.PopLimit(msgLimit);

    return true;
}

bool CCommClient::write(const ::google::protobuf::Message& message)
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
    if(!m_pProtocolClient->transmit(&pkt[0], siz))
        return false;

    return true;
}

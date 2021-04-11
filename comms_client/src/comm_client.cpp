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

#include <string>
#include <iostream>

#include "Logger.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


CCommClient::CCommClient()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    CLogger::GetInstance();

    m_pProtocolClient = std::make_shared<CTCPIPClient>();
//    m_pProtocol = std::make_shared<CPOSIX>();
//    m_pProtocol = std::make_shared<CCAN>();

}

CCommClient::~CCommClient()
{
    m_pProtocolClient->client_disconnect();
    google::protobuf::ShutdownProtobufLibrary();
}

bool CCommClient::connect()
{
    return m_pProtocolClient->client_connect();
}

bool CCommClient::read(::google::protobuf::Message& message)
{
    using namespace google::protobuf::io;

    int siz;
    char *pkt = nullptr;

    //fetch data
    if(!m_pProtocolClient->recieve(&pkt, siz)) {
        CLogger::Print(LOGLEV_RUN, "read.", " protocol recieve returned error");
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
//    std::cout << "size of test "<< sizeof(message) << std::endl;
//    std::cout << "size after serilizing is "<< message.ByteSizeLong() << std::endl;

    // serialise data
    std::uint32_t siz = message.ByteSizeLong();
    siz += CodedOutputStream::VarintSize32(siz);
    char pkt[siz]; // note: allowed in Linx, not windows
    google::protobuf::io::ArrayOutputStream aos(&pkt, siz);
    std::unique_ptr<CodedOutputStream> coded_output = std::make_unique<CodedOutputStream>(&aos);
    coded_output->WriteVarint32(message.ByteSizeLong());
    if(!message.SerializeToCodedStream(coded_output.get()))
        return false;

    // send data
    if(!m_pProtocolClient->transmit(pkt, siz))
        return false;

    return true;
}

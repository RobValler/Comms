/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "comms.h"
#include "tcpip.h"

#include <string>
#include <iostream>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


CComms::CComms()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    m_pProtocol = std::make_shared<CTCPIP>();

}

CComms::~CComms()
{
    m_pProtocol->disconnect();
    google::protobuf::ShutdownProtobufLibrary();
}

bool CComms::connect()
{

    return m_pProtocol->client_connect();
}

void CComms::read(::google::protobuf::Message& message)
{
    using namespace google::protobuf::io;

    //fetch data
    m_pProtocol->recieve();

    google::protobuf::io::ArrayInputStream ais(pkt,siz+4);
    CodedInputStream coded_input(&ais);
    coded_input.ReadVarint32(&siz);
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
    message.ParseFromCodedStream(&coded_input);
    coded_input.PopLimit(msgLimit);

    delete pkt;
}

void CComms::write(const ::google::protobuf::Message& message)
{
    using namespace google::protobuf::io;
    std::cout << "size of test "<< sizeof(message) << std::endl;
    std::cout << "size after serilizing is "<< message.ByteSizeLong() << std::endl;

    siz = message.ByteSizeLong()+4;
    pkt = new char [siz];
    google::protobuf::io::ArrayOutputStream aos(pkt, siz);
    CodedOutputStream *coded_output = new CodedOutputStream(&aos);
    coded_output->WriteVarint32(message.ByteSizeLong());
    message.SerializeToCodedStream(coded_output);

    // send data
    m_pProtocol->transmit();

}

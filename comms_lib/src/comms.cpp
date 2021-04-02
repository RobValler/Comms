/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "comms.h"
#include "iprotocol.h"
#include "example.pb.h"

#include <string>
#include <iostream>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>


std::uint32_t siz;
char *pkt;

CComms::CComms()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

}

CComms::~CComms()
{
    google::protobuf::ShutdownProtobufLibrary();
}

void CComms::read()
{
    using namespace google::protobuf::io;

    test_msg test;

    google::protobuf::io::ArrayInputStream ais(pkt,siz+4);
    CodedInputStream coded_input(&ais);
    coded_input.ReadVarint32(&siz);
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
    test.ParseFromCodedStream(&coded_input);
    coded_input.PopLimit(msgLimit);
    int result_int = test.test_int();
    std::string result_string = test.test_string();
}

void CComms::write()
{
    using namespace google::protobuf::io;

    test_msg test;
    test.set_test_string("moose");
    test.set_test_int(123);
    std::cout << "size of test "<< sizeof(test) << std::endl;
    std::cout << "size after serilizing is "<< test.ByteSizeLong() << std::endl;

    siz = test.ByteSizeLong()+4;
    pkt = new char [siz];
    google::protobuf::io::ArrayOutputStream aos(pkt, siz);
    CodedOutputStream *coded_output = new CodedOutputStream(&aos);
    coded_output->WriteVarint32(test.ByteSizeLong());
    test.SerializeToCodedStream(coded_output);
}

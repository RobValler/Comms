/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "proto_helper.h"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

namespace comms {
namespace serial {
namespace protobuf {

CSerialiserHelper::CSerialiserHelper()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

CSerialiserHelper::~CSerialiserHelper()
{
    google::protobuf::ShutdownProtobufLibrary();
}

bool CSerialiserHelper::serialise(std::vector<char>& buffer, int& size_of_message, void* incomming_data)
{
    using namespace google::protobuf::io;

    if(nullptr == incomming_data)
        return false;

    ::google::protobuf::Message* message = static_cast<::google::protobuf::Message*>(incomming_data);

    // serialise data
    size_of_message = message->ByteSizeLong();
    size_of_message += CodedOutputStream::VarintSize32(size_of_message);
    buffer.resize(size_of_message);
    google::protobuf::io::ArrayOutputStream aos(&buffer[0], size_of_message);
    CodedOutputStream coded_output(&aos);
    coded_output.WriteVarint32(message->ByteSizeLong());

    if(!message->SerializeToCodedStream(&coded_output))
        return false;

    return true;
}

bool CSerialiserHelper::deserialise(const std::vector<char>& buffer, int size_of_message, void* outgoing_data)
{
    using namespace google::protobuf::io;

    if(nullptr == outgoing_data)
        return false;

    ::google::protobuf::Message* message = static_cast<::google::protobuf::Message*>(outgoing_data);

    // convert from serialised char array to protobuf message class
    google::protobuf::io::ArrayInputStream ais(&buffer[0], size_of_message);
    CodedInputStream coded_input(&ais);
    std::uint32_t size = static_cast<std::uint32_t>(size_of_message);
    coded_input.ReadVarint32(&size);
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(size);
    message->ParseFromCodedStream(&coded_input);
    coded_input.ConsumedEntireMessage();
    coded_input.PopLimit(msgLimit);

    return true;
}

} // protobuf
} // serial
} // comms

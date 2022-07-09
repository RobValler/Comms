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

CSerialiserProto::CSerialiserProto()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

CSerialiserProto::~CSerialiserProto()
{
    google::protobuf::ShutdownProtobufLibrary();
}

bool CSerialiserProto::serialise(void* incomming_data, std::vector<char>& outgoing_data, int& outgoing_size)
{
    using namespace google::protobuf::io;

    if(nullptr == incomming_data)
        return false;

    ::google::protobuf::Message* message = static_cast<::google::protobuf::Message*>(incomming_data);

    // serialise data
    outgoing_size = message->ByteSizeLong();
    outgoing_size += CodedOutputStream::VarintSize32(outgoing_size);
    outgoing_data.resize(outgoing_size);
    google::protobuf::io::ArrayOutputStream aos(&outgoing_data[0], outgoing_size);
    CodedOutputStream coded_output(&aos);
    coded_output.WriteVarint32(message->ByteSizeLong());

    if(!message->SerializeToCodedStream(&coded_output))
        return false;

    return true;
}

bool CSerialiserProto::deserialise(const std::vector<char>& incomming_data, void* outgoing_data, int& outgoing_size)
{
    using namespace google::protobuf::io;

    if(nullptr == outgoing_data)
        return false;

    ::google::protobuf::Message* message = static_cast<::google::protobuf::Message*>(outgoing_data);

    // convert from serialised char array to protobuf message class
    google::protobuf::io::ArrayInputStream ais(&incomming_data[0], outgoing_size);
    CodedInputStream coded_input(&ais);
    std::uint32_t size = static_cast<std::uint32_t>(outgoing_size);
    coded_input.ReadVarint32(&size);
    outgoing_size = size;
    google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(size);
    message->ParseFromCodedStream(&coded_input);
    coded_input.ConsumedEntireMessage();
    coded_input.PopLimit(msgLimit);

    return true;
}

} // protobuf
} // serial
} // comms

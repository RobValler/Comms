/*****************************************************************
 * Copyright (C) 2017-2019 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/repeated_field.h>



#include "comm_client.h"
#include "example.pb.h"
#include "common.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

TEST(Comms_client_TCPIP, LargeDataWriteThenRead)
{

    const int val = 460800U;
    int size;
    std::vector<char> buffer_out(val);
    std::vector<char> buffer_in(val);
    CCommClient client(client_proto::EPT_TCTPIP, client_proto::EST_None);

    ASSERT_EQ(client.connect("127.0.0.1"), true);

    // write all ones to comparison buffer
    for(auto& it : buffer_out){
        it = 1U;
    }

    for(int index = 0; index < 1000; ++index)
    {
        std::this_thread::sleep_for( std::chrono::microseconds(1500));

        buffer_in={};
        buffer_in.resize(val);
        EXPECT_EQ(client.read(&buffer_in[0], size), true);
        EXPECT_EQ(buffer_in.size(), size);
        EXPECT_EQ(buffer_in, buffer_out);
    }
}

TEST(Comms_client_TCPIP, LargeDataWriteThenReadProto)
{

    const int val = 460800U;
    int size;
    std::vector<char> buffer_in(val);
    std::vector<char> buffer_out(val);
    CCommClient client(client_proto::EPT_TCTPIP, client_proto::EST_PROTO);
    test_msg in, out;

    ASSERT_EQ(client.connect("127.0.0.1"), true);

    // write all ones to comparison buffer
    for(auto& it : buffer_out) {
        it = 1U;
    }
    *out.mutable_data() = {buffer_out.begin(), buffer_out.end()};

    int index = 0;
    while(index < 10)
    {
        std::this_thread::sleep_for( std::chrono::milliseconds(100));

        in.Clear();
        if(client.numOfMessages() > 0)
        {
            EXPECT_EQ(client.read(&in, size), true);
            EXPECT_EQ(in.test_string(), "moose");
            const google::protobuf::RepeatedField<int32_t> & myField1 = in.data();
            const google::protobuf::RepeatedField<int32_t> & myField2 = out.data();
            EXPECT_EQ(true, std::equal(myField1.begin(), myField1.end(), myField2.begin()));
            index++;
            std::cout << "message read!! " << index << std::endl;
        }
        else
        {
            std::cout << "message NOT read!! " << index << std::endl;
        }
    }
}

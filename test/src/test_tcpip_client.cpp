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

TEST(Comms_client_TCPIP, LargeDataWriteThenReadBasic)
{
    std::vector<char> buffer_out(sizeOfData);
    std::vector<char> buffer_in(sizeOfData);
    CCommClient client(client_proto::EPT_TCTPIP, client_proto::EST_None);

    ASSERT_EQ(client.connect("127.0.0.1"), true);

    // write all ones to comparison buffer
    for(auto& it : buffer_out){
        it = 1U;
    }

    int size = 0;
    int index = 0;
    int numOfTimesNotRead = 0;
    while(index < numOfLoops)
    {
        std::this_thread::sleep_for( std::chrono::microseconds(delayBetweenReads_usec));

        if(client.numOfMessages() > 0)
        {
            buffer_in={};
            buffer_in.resize(sizeOfData);
            EXPECT_EQ(client.read(&buffer_in[0], size), true);
            EXPECT_EQ(buffer_in.size(), size);
            EXPECT_EQ(buffer_in, buffer_out);
            index++;
            std::cout << "read " << index << std::endl;
        }
        else
        {
            numOfTimesNotRead++;
            //std::cout << "not read" << std::endl;
        }
    }

    std::cout << "Number of failed reads " << numOfTimesNotRead << std::endl;
}

TEST(Comms_client_TCPIP, LargeDataWriteThenReadProto)
{
    int size;
    std::vector<char> buffer_in(sizeOfData);
    std::vector<char> buffer_out(sizeOfData);
    CCommClient client(client_proto::EPT_TCTPIP, client_proto::EST_PROTO);
    test_msg in, out;

    ASSERT_EQ(client.connect("127.0.0.1"), true);

    // write all ones to comparison buffer
    for(auto& it : buffer_out) {
        it = 1U;
    }
    *out.mutable_data() = {buffer_out.begin(), buffer_out.end()};

    int index = 0;
    int numOfTimesNotRead = 0;
    while(index < numOfLoops)
    {
        std::this_thread::sleep_for( std::chrono::microseconds(delayBetweenReads_usec));

        if(client.numOfMessages() > 0)
        {
            in.Clear();
            EXPECT_EQ(client.read(&in, size), true);
            //EXPECT_EQ(in.test_string(), "moose");
            const google::protobuf::RepeatedField<int32_t> & myField1 = in.data();
            const google::protobuf::RepeatedField<int32_t> & myField2 = out.data();
            EXPECT_EQ(true, std::equal(myField1.begin(), myField1.end(), myField2.begin()));
            index++;
        }
        else
        {
            numOfTimesNotRead++;
        }
    }

    std::cout << "Number of failed reads " << numOfTimesNotRead << std::endl;
}

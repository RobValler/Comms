/*****************************************************************
 * Copyright (C) 2017-2019 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

#include "comm_server.h"
#include "example.pb.h"
#include "common.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

TEST(Comms_server_TCPIP, LargeDataWriteThenRead)
{
    const int val = 460800U;
    std::vector<char> buffer_out(val);
    CCommServer server(server_proto::EPT_TCTPIP, server_proto::EST_None);

    ASSERT_EQ(server.init(), true);

    // write all ones to buffer
    for(auto& it : buffer_out){
        it = 1U;
    }

    for(int index = 0; index < 1000; ++index)
    {
        std::this_thread::sleep_for( std::chrono::microseconds(500) );

        EXPECT_EQ(server.write(buffer_out.data(), buffer_out.size()), true);
    }
}


TEST(Comms_server_TCPIP, LargeDataWriteThenReadProto)
{
    const int val = 460800U;
    std::vector<char> buffer_out(val);
    CCommServer server(server_proto::EPT_TCTPIP, server_proto::EST_PROTO);
    test_msg in, out;

    ASSERT_EQ(server.init(), true);

    // write all ones to buffer
    for(auto& it : buffer_out){
        it = 1U;
    }

    *out.mutable_data() = {buffer_out.begin(), buffer_out.end()};
    out.set_test_string("moose");

    for(int index = 0; index < 10; ++index)
    {
        std::this_thread::sleep_for( std::chrono::microseconds(500) );
        ASSERT_EQ(server.write(&out), true);
    }
}

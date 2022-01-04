/*****************************************************************
 * Copyright (C) 2017-2019 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

#include "comm_client.h"
//#include "example.pb.h"
#include "common.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

TEST(Comms_client_TCPIP, LargeDataWriteThenRead)
{
    const int val = 460800U;
    std::vector<char> buffer_out(val);
    std::vector<char> buffer_in(val);
    CCommClient client(client_proto::EPT_TCTPIP, client_proto::EST_None);

    ASSERT_EQ(client.connect("127.0.0.1"), true);

    // write all ones to comparison buffer
    for(auto& it : buffer_out){
        it = 1U;
    }

    for(int index = 0; index < 100; ++index)
    {
        std::this_thread::sleep_for( std::chrono::milliseconds(10) );

        buffer_in={};
        buffer_in.resize(val);
        EXPECT_EQ(client.read(&buffer_in[0]), true);
        EXPECT_EQ(buffer_in, buffer_out);
    }
}

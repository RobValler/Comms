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
//#include "example.pb.h"
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

    ASSERT_EQ(server.create(), true);

    // write all ones to buffer
    for(auto& it : buffer_out){
        it = 1U;
    }

    for(int index = 0; index < 100; ++index)
    {
        std::this_thread::sleep_for( std::chrono::milliseconds(5) );

        EXPECT_EQ(server.write(buffer_out.data(), buffer_out.size()), true);
    }
}

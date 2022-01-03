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

    //const int val = 460800U;
    const int val = 2048U;
    //const int val = 65528U;
    std::vector<char> buffer_out(val);
    std::vector<char> buffer_in(val);
    CCommClient client(client_proto::EPT_TCTPIP, client_proto::EST_None);

    ASSERT_EQ(client.connect("127.0.0.1"), true);

//    // write all ones to buffer
//    for(auto& it : buffer_out){
//        it = 1U;
//    }

    while(true)
    {
        std::this_thread::sleep_for( std::chrono::milliseconds(5) );

        buffer_in={};
        buffer_in.resize(val);
//        bool result = client.read(&buffer_in[0]);
//        EXPECT_EQ(result, true);
//        if(false == result)
//            break;

//        EXPECT_EQ(buffer_in, buffer_out);
    }
}

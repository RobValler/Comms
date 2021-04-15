/*****************************************************************
 * Copyright (C) 2017-2019 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

#include "comm_server.h"
#include "comm_client.h"
#include "example.pb.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

namespace  {
    const std::string out_str = "flap jacks";
    const std::int32_t out_int = 21345;
}

TEST(Comms, BasicTest_TCPIP)
{
    CCommServer server;
    CCommClient client;
    test_msg in, out;
    bool result;

    // set test data
    out.set_test_int(out_int);
    out.set_test_string(out_str);

    std::this_thread::sleep_for( std::chrono::milliseconds(500) );
    result = client.connect("127.0.0.1");
    EXPECT_EQ(result, true);
    if(!result)
        return;

    std::this_thread::sleep_for( std::chrono::milliseconds(200) );


    // write and read
    EXPECT_EQ(client.write(out), true);
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );
    EXPECT_EQ(server.read(in), true);

    // check return values
    EXPECT_EQ(out_int, in.test_int());
    EXPECT_EQ(out_str, in.test_string());
}

TEST(Comms, BasicTest_POSIXMQ)
{
    CCommServer server;

    test_msg in, out;
    bool result;



    // set test data
    out.set_test_int(out_int);
    out.set_test_string(out_str);

    std::this_thread::sleep_for( std::chrono::milliseconds(500) );




}


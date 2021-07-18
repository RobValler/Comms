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
#include "common.h"

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

namespace  {
    const std::string out_str = "flap jacks";
    const std::int32_t out_int = 21345;
}

TEST(Comms_TCPIP, ReadWrite)
{
    CCommServer server(ETCTPIP);
    CCommClient client(ETCTPIP);
    test_msg in, out;
    bool result;

    // set test data
    out.set_test_int(out_int);
    out.set_test_int_2(out_int + 1);
    out.set_test_string(out_str);

    std::this_thread::sleep_for( std::chrono::milliseconds(100) );
    result = client.connect("127.0.0.1");
    EXPECT_EQ(result, true);
    if(!result)
        return;

    std::this_thread::sleep_for( std::chrono::milliseconds(200) );

    // client write and server read
    EXPECT_EQ(client.write(out), true);
    std::this_thread::sleep_for( std::chrono::milliseconds(100) );
    EXPECT_EQ(server.read(in), true);

    EXPECT_EQ(out_int, in.test_int());
    EXPECT_EQ(out_int+1, in.test_int_2());
    EXPECT_EQ(out_str, in.test_string());




    // client read and server write
    in.Clear();
    EXPECT_EQ(server.write(out), true);
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );
    EXPECT_EQ(client.read(in), true);

    EXPECT_EQ(out_int, in.test_int());
    EXPECT_EQ(out_int+1, in.test_int_2());
    EXPECT_EQ(out_str, in.test_string());

}




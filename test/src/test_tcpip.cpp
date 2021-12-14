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
    const std::int32_t out_int = 21345U;
}

TEST(Comms_TCPIP, Connect)
{
    CCommClient client(ETCTPIP);
    ASSERT_EQ(client.connect("127.0.0.1"), false);
    CCommServer server(ETCTPIP);
    ASSERT_EQ(client.connect("127.0.0.1"), true);
}

TEST(Comms_TCPIP, ReadThenWrite)
{
    CCommServer server(ETCTPIP);
    CCommClient client(ETCTPIP);
    test_msg in, out;

    // set test data
    out.set_test_int(out_int);
    out.set_test_int_2(out_int + 1);
    out.set_test_string(out_str);

    ASSERT_EQ(client.connect("127.0.0.1"), true);

    for(int index=0; index < 1000; index++)
    {
       // std::cout << "----> Text iteration index = " << index +1 << std::endl;

        // client write and server read
        in.Clear();
        EXPECT_EQ(client.write(out), true);
        EXPECT_EQ(server.read(in), true);

        EXPECT_EQ(out_int, in.test_int());
        EXPECT_EQ(out_int+1, in.test_int_2());
        EXPECT_EQ(out_str, in.test_string());

        // server write and client read
        in.Clear();
        EXPECT_EQ(server.write(out), true);
        EXPECT_EQ(client.read(in), true);

        EXPECT_EQ(out_int, in.test_int());
        EXPECT_EQ(out_int+1, in.test_int_2());
        EXPECT_EQ(out_str, in.test_string());
    }
}

TEST(Comms_TCPIP, WriteOneReadMany)
{
    CCommServer server(ETCTPIP);
    CCommClient client(ETCTPIP);
    test_msg in, out;

    out.set_test_int(out_int);
    ASSERT_EQ(client.connect("127.0.0.1"), true);
    EXPECT_EQ(client.write(out), true);
    for(int index=0; index < 20; index++)
    {
        in.Clear();
        if(0 == index)
            EXPECT_EQ(server.read(in), true);
        else
            EXPECT_EQ(server.read(in), false);
    }
}

TEST(Comms_TCPIP, WriteManyThenReadMany)
{
    CCommServer server(ETCTPIP);
    CCommClient client(ETCTPIP);
    test_msg in, out;
    const int numberOfWrites = 1000;

    ASSERT_EQ(client.connect("127.0.0.1"), true);

    // writes
    for(int index=0; index < numberOfWrites; ++index)
    {
        out.set_test_int(index);
        EXPECT_EQ(client.write(out), true);

        ///\ todo get rid of this delay
        std::this_thread::sleep_for( std::chrono::microseconds(10) );
    }

    EXPECT_EQ(server.sizeOfReadBuffer(), numberOfWrites);

    // reads
    for(int index=0; index < numberOfWrites; ++index)
    {
        in.Clear();
        EXPECT_EQ(server.read(in), true);
        EXPECT_EQ(index, in.test_int());
    }
}

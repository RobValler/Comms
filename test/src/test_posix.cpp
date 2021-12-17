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

TEST(Comms_POSIX_MQ, Connect)
{
    CCommClient client(EPOSIX_MQ);
    ASSERT_EQ(client.connect("/posix_test_mq"), false);
    CCommServer server(EPOSIX_MQ);
    ASSERT_EQ(client.connect("/posix_test_mq"), true);
}

TEST(Comms_POSIX_MQ, ReadThenWrite)
{
    CCommServer server(EPOSIX_MQ);
    CCommClient client(EPOSIX_MQ);
    test_msg in, out;

    // set test data
    out.set_test_int(out_int);
    out.set_test_int_2(out_int + 1);
    out.set_test_string(out_str);

    ASSERT_EQ(client.connect("/posix_test_mq"), true);
    std::this_thread::sleep_for( std::chrono::microseconds(500) );


    EXPECT_EQ(client.write(&out), true);
    std::this_thread::sleep_for( std::chrono::microseconds(50) );
    EXPECT_EQ(server.read(&in), true);

    std::this_thread::sleep_for( std::chrono::seconds(1) );

//    EXPECT_EQ(out_int, in.test_int());
//    EXPECT_EQ(out_int+1, in.test_int_2());
//    EXPECT_EQ(out_str, in.test_string());

//    for(int index=0; index < 1; index++)
//    {
//        // client write and server read
//        in.Clear();
//        EXPECT_EQ(client.write(&out), true);
//        EXPECT_EQ(server.read(&in), true);

//        EXPECT_EQ(out_int, in.test_int());
//        EXPECT_EQ(out_int+1, in.test_int_2());
//        EXPECT_EQ(out_str, in.test_string());

//        // server write and client read
//        in.Clear();
//        EXPECT_EQ(server.write(&out), true);
//        EXPECT_EQ(client.read(&in), true);

//        EXPECT_EQ(out_int, in.test_int());
//        EXPECT_EQ(out_int+1, in.test_int_2());
//        EXPECT_EQ(out_str, in.test_string());
//    }
}

TEST(Comms_POSIX_MQ, WriteOneReadMany)
{
    CCommServer server(EPOSIX_MQ);
    CCommClient client(EPOSIX_MQ);
    test_msg in, out;

    out.set_test_int(out_int);
    ASSERT_EQ(client.connect("/posix_test_mq"), true);
    EXPECT_EQ(client.write(&out), true);
    for(int index=0; index < 20; index++)
    {
        in.Clear();
        if(0 == index)
            EXPECT_EQ(server.read(&in), true);
        else
            EXPECT_EQ(server.read(&in), false);
    }
}

TEST(Comms_POSIX_MQ, WriteManyThenReadMany)
{
    CCommServer server(EPOSIX_MQ);
    CCommClient client(EPOSIX_MQ);
    test_msg in, out;
    const int numberOfWrites = 1000;

    ASSERT_EQ(client.connect("/posix_test_mq"), true);

    // writes
    for(int index=0; index < numberOfWrites; ++index)
    {
        out.set_test_int(index);
        EXPECT_EQ(client.write(&out), true);

        ///\ todo get rid of this delay
        std::this_thread::sleep_for( std::chrono::microseconds(50) );
    }

    EXPECT_EQ(server.sizeOfReadBuffer(), numberOfWrites);

    // reads
    for(int index=0; index < numberOfWrites; ++index)
    {
        in.Clear();
        EXPECT_EQ(server.read(&in), true);
        EXPECT_EQ(index, in.test_int());
    }
}
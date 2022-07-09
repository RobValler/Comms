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
    const std::string server_name("/posix_test_mq_server");
    const std::string client_name("/posix_test_mq_client");
}

TEST(Comms_POSIX_MQ, Flush)
{
    CCommClient client(client_proto::EPT_POSIX_MQ, client_proto::EST_PROTO);
    CCommServer server(server_proto::EPT_POSIX_MQ, server_proto::EST_PROTO);
}

TEST(Comms_POSIX_MQ, Connect)
{
    CCommClient client(client_proto::EPT_POSIX_MQ, client_proto::EST_PROTO);
    ASSERT_EQ(client.connect(server_name), false);
    CCommServer server(server_proto::EPT_POSIX_MQ, server_proto::EST_PROTO);
    ASSERT_EQ(client.connect(server_name), true);
}

TEST(Comms_POSIX_MQ, ReadThenWrite)
{
    CCommServer server(server_proto::EPT_POSIX_MQ, server_proto::EST_PROTO);
    CCommClient client(client_proto::EPT_POSIX_MQ, client_proto::EST_PROTO);
    test_msg in;
    test_msg out;

    // set test data
    out.set_test_int(out_int);
    out.set_test_int_2(out_int + 1);
    out.set_test_string(out_str);

    ASSERT_EQ(client.connect(server_name), true);
    ASSERT_EQ(server.connect(client_name), true);

    for(int index=0; index < 1000; ++index)
    {
        // client write and server read
        in.Clear();
        EXPECT_EQ(client.write(&out), true);
        EXPECT_EQ(server.read(&in), true);

        EXPECT_EQ(out_int, in.test_int());
        EXPECT_EQ(out_int+1, in.test_int_2());
        EXPECT_EQ(out_str, in.test_string());

        // server write and client read
        in.Clear();
        EXPECT_EQ(server.write(&out), true);
        EXPECT_EQ(client.read(&in), true);

        EXPECT_EQ(out_int, in.test_int());
        EXPECT_EQ(out_int+1, in.test_int_2());
        EXPECT_EQ(out_str, in.test_string());
    }
}

TEST(Comms_POSIX_MQ, WriteOneReadMany)
{
    CCommServer server(server_proto::EPT_POSIX_MQ, server_proto::EST_PROTO);
    CCommClient client(client_proto::EPT_POSIX_MQ, client_proto::EST_PROTO);
    test_msg in, out;

    out.set_test_int(out_int);
    ASSERT_EQ(client.connect(server_name), true);
    EXPECT_EQ(client.write(&out), true);
    for(int index=0; index < 20; ++index)
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
    CCommServer server(server_proto::EPT_POSIX_MQ, server_proto::EST_PROTO);
    CCommClient client(client_proto::EPT_POSIX_MQ, client_proto::EST_PROTO);
    test_msg in, out;
    const int numberOfWrites = 1000;

    ASSERT_EQ(client.connect(server_name), true);
    ASSERT_EQ(server.connect(client_name), true);

    // writes
    for(int index=0; index < numberOfWrites; ++index)
    {
        out.set_test_int(index);
        EXPECT_EQ(client.write(&out), true);

        ///\ todo get rid of this delay
        std::this_thread::sleep_for( std::chrono::microseconds(50) );
    }

    //EXPECT_EQ(server.sizeOfReadBuffer(), numberOfWrites);

    // reads
    for(int index=0; index < numberOfWrites; ++index)
    {
        in.Clear();
        EXPECT_EQ(server.read(&in), true);
        EXPECT_EQ(index, in.test_int());
    }
}

TEST(Comms_POSIX_MQ, LargeDataWriteThenRead)
{
    //const int val = 460800U;
    //const int val = 65536U;
    const int val = 512;

    std::vector<char> buffer_out(val);
    std::vector<char> buffer_in(val);
    CCommServer server(server_proto::EPT_POSIX_MQ, server_proto::EST_PROTO);
    CCommClient client(client_proto::EPT_POSIX_MQ, client_proto::EST_PROTO);
    test_msg in, out;

    ASSERT_EQ(client.connect(server_name), true);

    *out.mutable_data() = {buffer_out.begin(), buffer_out.end()};
    EXPECT_EQ(client.write(&out), true);

//    std::this_thread::sleep_for( std::chrono::milliseconds(100) );

    EXPECT_EQ(server.read(&in), true);

}

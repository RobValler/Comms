/*****************************************************************
 * Copyright (C) 2017-2019 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

#include "comms.h"
#include "example.pb.h"

#include <string>
#include <chrono>
#include <thread>

TEST(Comms, BasicTest)
{
    CComms server, client;
    test_msg in, out;
    const std::string out_str = "flap jacks";
    const int out_int = 21345;

    // set test data
    out.set_test_int(out_int);
    out.set_test_string(out_str);

    std::this_thread::sleep_for( std::chrono::milliseconds(200) );
    EXPECT_EQ(client.connect(), true);

    // write and read
    client.write(out);
    server.read(in);

    std::this_thread::sleep_for( std::chrono::seconds(2) );

    // check return values
    EXPECT_EQ(in.test_int(), out_int);
    EXPECT_EQ(in.test_string(), out_str);
}

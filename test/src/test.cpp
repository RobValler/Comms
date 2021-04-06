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
#include <iostream>

TEST(Comms, BasicTest)
{
    CComms server, client;
    test_msg in, out;
    const std::string out_str = "flap jacks";
    const std::int32_t out_int = 21345;

    // set test data
    out.set_test_int(out_int);
    out.set_test_string(out_str);

    std::this_thread::sleep_for( std::chrono::milliseconds(200) );
    EXPECT_EQ(client.connect(), true);

    // write and read
    EXPECT_EQ(client.write(out), true);
    std::this_thread::sleep_for( std::chrono::milliseconds(300) );
    EXPECT_EQ(server.read(in), true);

//    std::cout << "out = " << out_int << ", in = " << in.test_int() << std::endl;
//    std::cout << "out = " << out_str << ", in = " << in.test_string() << std::endl;

    // check return values
    EXPECT_EQ(out_int, in.test_int());
    EXPECT_EQ(out_str, in.test_string());
}

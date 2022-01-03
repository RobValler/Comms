/*****************************************************************
 * Copyright (C) 2017-2019 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
//    ::testing::GTEST_FLAG(filter) = "Comms_POSIX_MQ.Flush";   // always run
//    ::testing::GTEST_FLAG(filter) = "-Comms_TCPIP.Connect";
//    ::testing::GTEST_FLAG(filter) = "Comms_POSIX_MQ.Connect";

//    ::testing::GTEST_FLAG(filter) = "*.ReadThenWrite";
    //::testing::GTEST_FLAG(filter) = "Comms_POSIX_MQ.LargeDataWriteThenRead";
    ::testing::GTEST_FLAG(filter) = "Comms_TCPIP.LargeDataWriteThenRead";
    //::testing::GTEST_FLAG(filter) = "Comms_TCPIP.Connect";


    return RUN_ALL_TESTS();
}

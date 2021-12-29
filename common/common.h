/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <string>

enum EMessageType : std::uint8_t
{
    EMsgTypNone = 0,
    EMsgTypCtrl,
    EMsgTypData
};

struct SMessageHeader {
    std::uint32_t size;
    std::uint8_t type;
};

const std::string l_mq_channel_name("/posix_test_mq"); ///\ todo replace with parameter
const int l_max_num_of_connect_attempts = 5;

namespace posix_conf
{
    constexpr std::uint32_t max_size = 33;
    constexpr std::uint32_t max_msg = 10;
    constexpr std::uint32_t permission = 0644;
}


#endif //  COMMON_H

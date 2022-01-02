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

///\ todo sort the padding out for linux (and windows)
struct SMessageHeader {
    std::uint8_t instID;                ///< ID for this message instance
    std::uint32_t frame_size;           ///< message size size
    std::uint32_t max_size;             ///< total message size
    std::uint8_t type;                  ///< message type. data or control
    std::uint8_t frame_no;              ///< curret frame
    std::uint8_t max_frame_no;          ///< number of frames for this message
};

const std::string l_mq_channel_name("/posix_test_mq"); ///\ todo replace with parameter
const int l_max_num_of_connect_attempts = 5;

namespace posix_conf
{
    constexpr std::uint32_t msg_size = 1024;
    constexpr std::uint32_t max_msg = 10;
    constexpr std::uint32_t permission = 0644;
}

namespace tcpip_conf
{
    const std::int32_t max_msg_size_allowable = 1*1024*1024*1024;
    constexpr std::uint32_t msg_frame_size = 65520; // 2^16 - sizeof(SMessageHeader)
    //constexpr std::uint32_t msg_frame_size = 10224U;
}

#endif //  COMMON_H

/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

#include <cstdint>
#include <vector>

namespace comms {
namespace tcpip {
namespace common {

struct SMessageHeader {
    std::uint32_t size;
    std::uint8_t type;
};

enum EMessageType : std::uint8_t
{
    EMsgTypNone = 0,
    EMsgTypCtrl,
    EMsgTypData
};


class CTCPIP_Common
{
public:
    CTCPIP_Common();
    ~CTCPIP_Common()=default;

    bool listenForData();
    bool crecieve(char** data, int& size);
    bool ctransmit(const char *data, const int size);

protected:
    char m_buffer[1024] = {0}; //todo: replace with dynamic array
    int m_sizeOfHeader{0};
    int m_connection_fd{0};
    int m_connection_socket{0};
    int m_size{0};
};

} // comms
} // tcpip
} // common

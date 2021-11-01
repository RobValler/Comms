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
#include <queue>
#include <mutex>

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

struct SReadBufferQ
{
    std::vector<char> data;
};

class CTCPIP_Common
{
public:
    CTCPIP_Common();
    ~CTCPIP_Common()=default;

    bool listenForData(const int fd);
    bool crecieve(std::vector<char>& data, int& size);
    bool ctransmit(const int fd, const char *data, const int size);

protected:
    std::queue<SReadBufferQ> m_read_queue{};
    char m_buffer[1024] = {0}; //todo: replace with dynamic array
    SReadBufferQ m_buff;
    int m_sizeOfHeader{0};

    std::mutex m_recProtect;

//    int m_connection_fd{0};
//    int m_connection_socket{0};
    int m_size{0};
    int m_socket_type{0};
    bool m_blocking{true};
};

} // comms
} // tcpip
} // common

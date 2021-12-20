/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once
#include "common.h"
#include <cstdint>
#include <vector>
#include <queue>
#include <mutex>

namespace comms {
namespace tcpip {
namespace helper {

struct SReadBufferQ
{
    std::vector<char> payload;
};

class CTCPIPHelper
{
public:
    CTCPIPHelper();
    ~CTCPIPHelper()=default;

    bool crecieve(std::vector<char>& data, int& size);
    bool ctransmit(const int fd, const char *data, const int size);
    int csizeOfReadBuffer();

protected:
    bool listenForData(const int fd);

    std::mutex m_recProtect;
    std::queue<SReadBufferQ> m_read_queue{};
    SReadBufferQ m_fetch_buffer;
    SReadBufferQ m_read_buffer;

    int m_sizeOfHeader{0};
    int m_socket_type{0};
    bool m_blocking{true};

    std::vector<char> m_transmitPackage;
    SMessageHeader m_read_header;
    SMessageHeader m_write_header;
};

} // helper
} // tcpip
} // comms

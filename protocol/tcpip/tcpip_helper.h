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
    std::vector<char> payload;
};

class CTCPIPHelper
{
public:
    CTCPIPHelper();
    ~CTCPIPHelper()=default;

    bool listenForData(const int fd);
    bool crecieve(std::vector<char>& data, int& size);
    bool ctransmit(const int fd, const char *data, const int size);
    int csizeOfReadBuffer();

protected:
    std::mutex m_recProtect;
    std::queue<SReadBufferQ> m_read_queue{};

    int m_sizeOfHeader{0};
    int m_socket_type{0};
    bool m_blocking{true};

    std::vector<char> m_transmitPackage;
};

} // comms
} // tcpip
} // common

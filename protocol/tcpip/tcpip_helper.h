/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
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
#include <atomic>

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
    CTCPIPHelper(const CTCPIPHelper&) = delete;                // Copy constructor
    CTCPIPHelper(CTCPIPHelper&&) = delete;                     // Move constructor
    CTCPIPHelper& operator=(const CTCPIPHelper&) = delete;     // Copy assignment operator
    CTCPIPHelper& operator=(CTCPIPHelper&&) = delete;          // Move assignment operator
    virtual ~CTCPIPHelper();                                   // Destructor

    bool crecieve(std::vector<char>& data, int& size);
    bool ctransmit(const int fd, const char *data, const int size);
    int csizeOfReadBuffer();

protected:
    bool listenForData(const int fd);
    bool writeData(const int fd);

    std::atomic<bool> m_shutdownRequest{false};

    int m_sizeOfHeader{0};
    int m_socket_type{0};
    bool m_blocking{true};

    // buffers read
    std::queue<SReadBufferQ> m_read_queue{};
    SReadBufferQ m_read_data_buffer{};
    SMessageHeader m_read_header{};
    std::vector<char> m_read_frame_buffer{};
    std::mutex m_readQueueProtect;

    // buffers write
    std::queue<SReadBufferQ> m_write_queue{};
    SReadBufferQ m_write_data_buffer{};
    SMessageHeader m_write_header{};
    std::vector<char> m_write_frame_buffer{};
    std::mutex m_writeQueueProtect;
};

} // helper
} // tcpip
} // comms

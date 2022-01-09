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
    virtual ~CTCPIPHelper() =default;                          // Destructor

    bool crecieve(const int fd, std::vector<char>& data, int& size);
    bool ctransmit(const int fd, const char *data, const int size);

protected:
    int m_sizeOfHeader{0};
    int m_socket_type{0};

    SMessageHeader m_read_header{};
    SMessageHeader m_write_header{};

};

} // helper
} // tcpip
} // comms

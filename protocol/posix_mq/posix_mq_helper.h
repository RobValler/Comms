/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once
#include "common.h"

#include <vector>
#include <queue>
#include <mutex>

#include <mqueue.h>

namespace comms {
namespace posix {
namespace helper {

struct SReadBufferQ
{
    std::vector<char> payload;
};

class CPOSIXMQHelper
{
public:
    CPOSIXMQHelper()=default;
    ~CPOSIXMQHelper()=default;

protected:
    bool channel_create(std::string name);
    bool cclient_connect(std::string channel);
    bool cclient_disconnect();
    bool crecieve(std::vector<char>& data, int& size);
    bool ctransmit(const mqd_t m_mqdes, const char *data, const int size);
    bool listenForData(const mqd_t m_mqdes);

    int m_sizeOfHeader{0};
    std::mutex m_recProtect;
    std::queue<SReadBufferQ> m_read_queue{};
    std::vector<char> m_transmitPackage;

    std::string m_provider_channel_name;
    mqd_t m_provider_channel_desc;
    mqd_t m_listener_channel_desc;
};

} // helper
} // posix
} // comms

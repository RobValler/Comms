/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

#include "iprotocol_server.h"
#include "posix_mq_helper.h"

#include <thread>
#include <atomic>

#include <mqueue.h>

namespace comms {
namespace posix {
namespace server {

class CPOSIXMQServer
        : public IProtocolServer
        , public helper::CPOSIXMQHelper
{
public:
    CPOSIXMQServer();
    ~CPOSIXMQServer();

    bool channel_create() override;
    bool recieve(std::vector<char>& data, int& size) override {
        return crecieve(data, size);
    }
    bool transmit(const char *data, const int size) override {
        return ctransmit(m_mqdes_server, data, size);
    }
    int sizeOfReadBuffer() override { return 0; }


private:
    void threadfunc_server();

    std::atomic<bool> m_shutdownrequest;
    mqd_t m_mqdes_server;
    int m_sizeOfHeader;

    std::thread t_server;

    std::string m_channel;
};

} // comms
} // posix
} // client

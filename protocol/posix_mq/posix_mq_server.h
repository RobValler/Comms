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

#include <thread>
#include <atomic>

#include <mqueue.h>

namespace comms {
namespace posix {
namespace server {

class CPOSIXMQServer : public IProtocolServer
{
public:
    CPOSIXMQServer();
    ~CPOSIXMQServer();

    bool channel_create() override;
    bool recieve(std::vector<char>&, int& size) override;
    bool transmit(const char *data, const int size) override;

private:
    void threadfunc_server();
    bool listenForData();

    std::atomic<bool> m_shutdownrequest;
    mqd_t m_msgQueue;
    int m_sizeOfHeader;

    std::thread t_server;
};

} // comms
} // posix
} // client

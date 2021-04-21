/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

#include "iprotocol_client.h"

#include <thread>
#include <atomic>

#include <mqueue.h>

namespace comms {
namespace posix {
namespace client {

class CPOSIXMQClient : public IProtocolClient
{
public:
    CPOSIXMQClient();
    ~CPOSIXMQClient();

    bool client_connect(std::string channel) override;
    bool client_disconnect() override;
    bool recieve(char** data, int& size) override;
    bool transmit(const char *data, const int size) override;

private:
    void threadfunc_client();
    bool listenForData();

    std::atomic<bool> m_shutdownrequest;
    mqd_t m_msgQueue;
    int m_sizeOfHeader;

    std::thread t_client;
};

} // comms
} // posix
} // client

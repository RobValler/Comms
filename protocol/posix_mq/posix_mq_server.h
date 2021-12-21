/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
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
#include <string>

#include <mqueue.h>

namespace comms {
namespace posix {
namespace server {

class CPOSIXMQServer
        : public IProtocolServer
        , public helper::CPOSIXMQHelper
{
public:
    CPOSIXMQServer();                                               // Constructor
    CPOSIXMQServer(const CPOSIXMQServer&) = delete;                 // Copy constructor
    CPOSIXMQServer(CPOSIXMQServer&&) = delete;                      // Move constructor
    CPOSIXMQServer& operator=(const CPOSIXMQServer&) = delete;      // Copy assignment operator
    CPOSIXMQServer& operator=(CPOSIXMQServer&&) = delete;           // Move assignment operator
    ~CPOSIXMQServer();                                              // Destructor

    bool client_connect(std::string channel) override { return cclient_connect(channel); }
    bool client_disconnect() override { return cclient_disconnect(); }
    bool recieve(std::vector<char>& data, int& size) override { return crecieve(data, size); }
    bool transmit(const char *data, const int size) override { return ctransmit(m_listener_channel_desc, data, size); }
    int sizeOfReadBuffer() override { return 0; }

private:
    void threadfunc_server();
    std::thread t_provider_channel_thread;
    std::atomic<bool> m_shutdownrequest;
};

} // comms
} // posix
} // client

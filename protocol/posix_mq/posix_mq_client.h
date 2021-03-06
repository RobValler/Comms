/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

#include "iprotocol_client.h"
#include "posix_mq_helper.h"

#include <thread>
#include <atomic>

namespace comms {
namespace posix {
namespace client {

class CPOSIXMQClient
        : public IProtocolClient
        , public helper::CPOSIXMQHelper
{
public:
    CPOSIXMQClient();                                               // Constructor
    CPOSIXMQClient(const CPOSIXMQClient&) = delete;                 // Copy constructor
    CPOSIXMQClient(CPOSIXMQClient&&) = delete;                      // Move constructor
    CPOSIXMQClient& operator=(const CPOSIXMQClient&) = delete;      // Copy assignment operator
    CPOSIXMQClient& operator=(CPOSIXMQClient&&) = delete;           // Move assignment operator
    ~CPOSIXMQClient();                                              // Destructor

    bool client_connect(std::string channel) override { return cclient_connect(channel); }
    bool client_disconnect() override { return cclient_disconnect(); }
    bool recieve(std::vector<char>& data, int& size) override { return crecieve(data, size); }
    bool transmit(const char *data, const int size) override { return ctransmit(m_listener_channel_desc, data, size); }

private:
    void threadfunc_client();
    std::thread t_provider_channel_thread;
    std::atomic<bool> m_shutdownrequest;
};

} // comms
} // posix
} // client

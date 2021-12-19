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

    bool client_connect(std::string channel) override;
    bool client_disconnect();
    bool recieve(std::vector<char>& data, int& size) override {
        return crecieve(data, size);
    }
    bool transmit(const char *data, const int size) override {
        return ctransmit(m_listener_channel_desc, data, size);
    }
    int sizeOfReadBuffer() override { return 0; }


private:
    void threadfunc_server();
    bool channel_create();

    std::atomic<bool> m_shutdownrequest;

    std::thread t_provider_channel_thread;
    std::string m_provider_channel_name;
    mqd_t m_provider_channel_desc;

    mqd_t m_listener_channel_desc;
};

} // comms
} // posix
} // client

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
#include "tcpip_helper.h"

#include <thread>
#include <atomic>
#include <vector>

namespace comms {
namespace tcpip {
namespace server {

class CTCPIPServer
        : public IProtocolServer
        , public common::CTCPIPHelper
{
public:
    CTCPIPServer();
    ~CTCPIPServer();

    bool channel_create() override;
    bool recieve(std::vector<char>& data, int& size) override{
        return crecieve(data, size);
    }
    bool transmit(const char *data, const int size) override{
        return ctransmit(m_connection_socket, data, size);
    }

private:
    void threadfunc_server();

    std::atomic<bool> m_shutdownrequest;
    std::thread t_server;

    int m_connection_fd{0};
    int m_connection_socket{0};

    std::vector<char> m_input_data_entry;
    std::vector<std::vector<char>> m_input_data_buffer;
};

} // comms
} // tcpip
} // server

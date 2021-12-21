/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

#include "iprotocol_client.h"
#include "tcpip_helper.h"

#include <thread>
#include <atomic>
#include <vector>
#include <string>

namespace comms {
namespace tcpip {
namespace client {

class CTCPIPClient
        : public IProtocolClient
        , public helper::CTCPIPHelper
{
public:
    CTCPIPClient();
    ~CTCPIPClient();

    bool client_connect(std::string ip_address) override;
    bool client_disconnect() override;
    bool recieve(std::vector<char>& data, int& size) override { return crecieve(data, size); }
    bool transmit(const char *data, const int size) override { return ctransmit(m_connection_fd, data, size); }

private:
    void threadfunc_client();

    std::atomic<bool> m_shutdownrequest;
    std::thread t_client;
    int m_connection_fd{0};

    std::vector<char> m_input_data_entry;
    std::vector<std::vector<char>> m_input_data_buffer;
};

} // comms
} // posix
} // client

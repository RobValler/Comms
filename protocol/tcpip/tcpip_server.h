/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
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
        , public helper::CTCPIPHelper
{
public:
    CTCPIPServer();
    ~CTCPIPServer();

    bool client_connect(std::string) override { return false; }
    bool client_disconnect() override {return false; }
    bool recieve(std::vector<char>& data, int& size) override { return crecieve(m_connection_socket, data, size); }
    bool transmit(const char *data, const int size) override { return ctransmit(m_connection_socket, data, size); }
    bool channel_create(std::string name) override;

private:
    std::atomic<bool> m_shutdownrequest;
    int m_connection_fd{0};
    int m_connection_socket{0};
};

} // comms
} // tcpip
} // server

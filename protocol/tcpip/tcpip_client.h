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
#include "tcpip_helper.h"

#include <thread>
#include <atomic>
#include <vector>

namespace comms {
namespace tcpip {
namespace client {

class CTCPIPClient
        : public IProtocolClient
        , public helper::CTCPIPHelper
{
public:
    CTCPIPClient();
    CTCPIPClient(const CTCPIPClient&) = delete;                // Copy constructor
    CTCPIPClient(CTCPIPClient&&) = delete;                     // Move constructor
    CTCPIPClient& operator=(const CTCPIPClient&) = delete;     // Copy assignment operator
    CTCPIPClient& operator=(CTCPIPClient&&) = delete;          // Move assignment operator
    ~CTCPIPClient();                                           // Destructor

    bool client_connect(std::string ip_address) override;
    bool client_disconnect() override;
    bool recieve(std::vector<char>& data, int& size) override { return crecieve(m_connection_fd, data, size); }
    bool transmit(const char *data, const int size) override { return ctransmit(m_connection_fd, data, size); }

private:
    std::atomic<bool> m_shutdownrequest{false};
    int m_connection_fd{0};
    std::vector<char> m_confirmMsgBuff;
};

} // comms
} // posix
} // client

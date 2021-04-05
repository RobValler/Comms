/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

#include "iprotocol.h"

#include <thread>
#include <atomic>

class CTCPIP : public IProtocol
{
public:
    CTCPIP();
    ~CTCPIP();

    bool server_connect() override;
    bool client_connect() override;
    bool disconnect() override;
    bool recieve(char** data, int& size) override;
    bool transmit(const char *data, const int size) override;

private:
    void threadfunc_server();
    void threadfunc_client();

    std::atomic<bool> m_shutdownrequest;
    std::thread t_server;
    std::thread t_client;

    int m_serverSocket;
    int m_clientSocket;
    int m_server_fd;

    char m_buffer[1024] = {0};
};

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
#include <vector>

class CTCPIPClient : public IProtocolClient
{
public:
    CTCPIPClient();
    ~CTCPIPClient();

    bool client_connect() override;
    bool client_disconnect() override;
    bool recieve(char** data, int& size) override;
    bool transmit(const char *data, const int size) override;

private:
    void threadfunc_client();
    bool listenForData();

    std::atomic<bool> m_shutdownrequest;
    std::thread t_client;
    int m_client_fd;
    int m_sizeOfHeader;

    char m_buffer[1024] = {0}; //todo: replace with dynamic array

    int m_size;

    std::vector<char> m_input_data_entry;
    std::vector<std::vector<char>> m_input_data_buffer;
};

/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once
#include <memory>
#include <vector>

class IProtocolClient;
class ISerialiser;

namespace client_proto {
    enum EProtocolType : unsigned int { ENone = 0, ETCTPIP, EPOSIX_MQ };
}

class CCommClient
{
public:
    CCommClient(client_proto::EProtocolType type);
    ~CCommClient();

    bool connect(std::string server_address);
    bool read(void* message);
    bool write(void* message);
    int numOfMessages();

private:
    std::unique_ptr<IProtocolClient> m_pProtocolClient;
    std::shared_ptr<ISerialiser> m_pSerialiser;

    std::vector<char> m_read_buffer;
    std::vector<char> m_write_buffer;
    int m_size_of_message{0};
};

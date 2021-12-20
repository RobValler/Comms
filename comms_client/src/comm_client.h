/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once
#include <memory>
#include <vector>

enum EProtocolType : unsigned int;
class IProtocolClient;
class ISerialiser;

class CCommClient
{
public:
    CCommClient(EProtocolType type);
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

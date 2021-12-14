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
};

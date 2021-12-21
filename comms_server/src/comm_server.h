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

class IProtocolServer;
class ISerialiser;

namespace server_proto {
    enum EProtocolType : unsigned int { ENone = 0, ETCTPIP, EPOSIX_MQ };
}

class CCommServer
{
public:    
    CCommServer(server_proto::EProtocolType type);
    ~CCommServer()=default;

    bool connect(std::string server_address);
    bool read(void* message);
    bool write(void* message);
    int sizeOfReadBuffer();

private:    
    std::shared_ptr<IProtocolServer> m_pProtocolServer;
    std::shared_ptr<ISerialiser> m_pSerialiser;

    std::vector<char> m_read_buffer;
    std::vector<char> m_write_buffer;
    int m_size_of_message{0};
};

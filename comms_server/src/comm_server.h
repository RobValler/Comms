/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once
#include "common.h"

#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>

class IProtocolServer;
class ISerialiser;

///\ todo namespace?

namespace server_proto {
    enum EProtocolType : unsigned int { EPT_None = 0, EPT_TCTPIP, EPT_POSIX_MQ };
    enum ESerialType : unsigned int { EST_None = 0, EST_PROTO };

    struct SReadBufferQ
    {
        std::vector<char> payload;
    };
}

class CCommServer
{
public:    
    CCommServer(server_proto::EProtocolType protocol, server_proto::ESerialType serial);
    ~CCommServer();

    bool connect(std::string server_address);
    bool read(void* message);
    bool write(void* message, int size = 0);
    int sizeOfReadBuffer();

private:
    // admin
    std::atomic<bool> m_shutdownrequest{false};

    // threads
    void createThread();
    void readThread();
    void writeThread();
    std::thread t_create;
    std::thread t_read;
    std::thread t_write;

    // delagates
    std::shared_ptr<IProtocolServer> m_pProtocolServer;
    std::shared_ptr<ISerialiser> m_pSerialiser;

    // buffers read
    std::queue<server_proto::SReadBufferQ> m_read_queue{};
    server_proto::SReadBufferQ m_read_container{};
    std::mutex m_readQueueProtect;

    // buffers write
    std::queue<server_proto::SReadBufferQ> m_write_queue{};
    server_proto::SReadBufferQ m_write_container{};
    std::mutex m_writeQueueProtect;
};

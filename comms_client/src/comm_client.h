/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>


class IProtocolClient;
class ISerialiser;

namespace client_proto {
    enum EProtocolType : unsigned int { EPT_None = 0, EPT_TCTPIP, EPT_POSIX_MQ };
    enum ESerialType : unsigned int { EST_None = 0, EST_PROTO };

    struct SReadBufferQ
    {
        std::vector<char> payload;
    };
}

class CCommClient
{
public:
    CCommClient(client_proto::EProtocolType protocol, client_proto::ESerialType serial);
    CCommClient(const CCommClient&) = delete;                // Copy constructor
    CCommClient(CCommClient&&) = delete;                     // Move constructor
    CCommClient& operator=(const CCommClient&) = delete;     // Copy assignment operator
    CCommClient& operator=(CCommClient&&) = delete;          // Move assignment operator
    ~CCommClient();                                          // Destructor

    bool connect(std::string server_address);
    bool read(void* message, int& size);
    bool write(void* message, int size = 0);
    int numOfMessages();

private:
    // admin
    std::atomic<bool> m_shutdownrequest{false};

    // threads
    void readThread();
    void writeThread();
    std::thread t_read;
    std::thread t_write;
    std::mutex cv_m;
    std::condition_variable cv_writeThread;

    // delagates
    std::shared_ptr<IProtocolClient> m_pProtocolClient;
    std::shared_ptr<ISerialiser> m_pSerialiser;

    // buffers read
    std::queue<client_proto::SReadBufferQ> m_read_queue{};
    client_proto::SReadBufferQ m_readcall_container{};
    client_proto::SReadBufferQ m_readthread_container{};
    std::mutex m_readQueueProtect;

    // buffers write
    std::queue<client_proto::SReadBufferQ> m_write_queue{};
    client_proto::SReadBufferQ m_writecall_container{};
    client_proto::SReadBufferQ m_writethread_container{};
    std::mutex m_writeQueueProtect;
};

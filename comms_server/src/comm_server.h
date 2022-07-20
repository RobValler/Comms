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
    CCommServer(const CCommServer&) = delete;                // Copy constructor
    CCommServer(CCommServer&&) = delete;                     // Move constructor
    CCommServer& operator=(const CCommServer&) = delete;     // Copy assignment operator
    CCommServer& operator=(CCommServer&&) = delete;          // Move assignment operator
    ~CCommServer();

    bool connect(std::string server_address);

/*! \fn bool init();
    \brief blocking call, block is released on successful connection or failure
    \return success/no success
*/
    bool init();
    bool read(void* message, int& size);
    bool write(void* message, int size = 0);
    int sizeOfReadBuffer();

private:
    // admin
    std::atomic<bool> m_shutdownrequest{false};

    // threads
    void readThread();
    void writeThread();
    void channelThread();
    std::thread t_read;
    std::thread t_write;
    std::thread t_channel;
    std::mutex cv_m;
    std::condition_variable cv_writeThread;

    // delagates
    std::shared_ptr<IProtocolServer> m_pProtocolServer;
    std::shared_ptr<ISerialiser> m_pSerialiser;

    // buffers read
    std::queue<server_proto::SReadBufferQ> m_read_queue{};
    server_proto::SReadBufferQ m_readcall_container{};
    server_proto::SReadBufferQ m_readthread_container{};
    std::mutex m_readQueueProtect;

    // buffers write
    std::queue<server_proto::SReadBufferQ> m_write_queue{};
    server_proto::SReadBufferQ m_writecall_container{};
    server_proto::SReadBufferQ m_writethread_container{};
    std::mutex m_writeQueueProtect;
};

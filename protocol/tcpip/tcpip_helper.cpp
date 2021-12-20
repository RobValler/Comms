/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_helper.h"

#include "Logger.h"

// tcp socket stuff
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include <chrono>
#include <thread>

namespace comms {
namespace tcpip {
namespace helper {

CTCPIPHelper::CTCPIPHelper()
{
    m_sizeOfHeader = sizeof(SMessageHeader);
}

bool CTCPIPHelper::crecieve(std::vector<char>& data, int& size)
{
    // spin counter
    const int max_spin_count = 15;
    for(int index=0; index < max_spin_count; ++index)
    {
        if(0 == m_read_queue.size())
        {
            if(index < max_spin_count - 2)
            {
                std::this_thread::sleep_for(std::chrono::nanoseconds(10));
                continue;
            }
            else
            {
                return false;
            }
        }
        else
        {
            break;
        }
    }

    m_fetch_buffer = {};
    m_recProtect.lock();
    m_fetch_buffer = m_read_queue.front();
    m_read_queue.pop();
    m_recProtect.unlock();
    data = m_fetch_buffer.payload;
    size = m_fetch_buffer.payload.size();

    if(size <= 0)
        return false;

    return true;
}

bool CTCPIPHelper::ctransmit(const int fd, const char *data, const int size)
{
    m_write_header.size = size;
    m_write_header.type = EMsgTypData;

    m_transmitPackage = {}; ///\ todo needed?
    m_transmitPackage.resize(size + m_sizeOfHeader);
    std::memcpy(&m_transmitPackage[0], &m_write_header, m_sizeOfHeader);
    std::memcpy(&m_transmitPackage[m_sizeOfHeader], data, size);

    if(0 < send(fd, &m_transmitPackage[0], m_transmitPackage.size() , 0 ))
        return true;
    else
        return false;
}

bool CTCPIPHelper::listenForData(const int fd)
{
    std::uint32_t numOfBytesRead;

    // Check the contents of the header
    //numOfBytesRead = recv( fd , &peekHeader, m_sizeOfHeader, peekFlags);
    numOfBytesRead = read( fd , &m_read_header, m_sizeOfHeader);
    if(numOfBytesRead <= 0) {
        CLOG(LOGLEV_RUN, "Header read failed, numOfBytesRead = ", numOfBytesRead);
        return false;
    }

    // check the data type
    if(EMsgTypData != m_read_header.type) {
        CLOG(LOGLEV_RUN, "wrong header type");
        return false;
    }

    // store the actual data
    m_read_buffer = {};
    m_read_buffer.payload.resize(m_read_header.size);
    numOfBytesRead = read(fd, m_read_buffer.payload.data(), m_read_buffer.payload.size());
    if(numOfBytesRead != m_read_header.size)
    {
        CLOG(LOGLEV_RUN, "read size did not match");
        return false;
    }
    else
    {
        m_recProtect.lock();
        m_read_queue.push(m_read_buffer);
        m_recProtect.unlock();
        //CLOG(LOGLEV_RUN, "message recieved of ", numOfBytesRead, " bytes");
    }

    return true;
}

int CTCPIPHelper::csizeOfReadBuffer()
{
    return m_read_queue.size();
}

} // helper
} // tcpip
} // comms

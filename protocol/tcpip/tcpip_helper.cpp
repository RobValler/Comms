/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_helper.h"
#include "common.h"
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

CTCPIPHelper::~CTCPIPHelper()
{
    m_shutdownRequest = true;
}

bool CTCPIPHelper::crecieve(std::vector<char>& data, int& size)
{
    // spin counter
    const int max_spin_count = 15;
    std::uint32_t queue_size;
    for(int index=0; index < max_spin_count; ++index)
    {
        m_readQueueProtect.lock();
        queue_size = m_read_queue.size();
        m_readQueueProtect.unlock();

        if(0 == queue_size)
        {
            if(index < max_spin_count - 2)
            {
                ///\ todo: possibly irrelevant under a certain duration
                std::this_thread::sleep_for(std::chrono::nanoseconds(10));
                continue;
            }
            else
            {
                CLOG(LOGLEV_RUN, "read queue is empty");
                return false;
            }
        }
        else
        {
            break;
        }
    }

    m_readQueueProtect.lock();
    data = m_read_queue.front().payload;
    m_read_queue.pop();
    m_readQueueProtect.unlock();
    size = data.size();

    if(size <= 0)
        return false;

    return true;
}

bool CTCPIPHelper::ctransmit(const int, const char *data, const int size)
{
    ///\ todo move multi-framer to helper class.
    if(size > tcpip_conf::max_msg_size_allowable)
    {
        CLOG(LOGLEV_RUN, "This message is too large, aborted!");
        return false;
    }

    std::uint8_t frame = 0;
    const std::int32_t frame_size = tcpip_conf::msg_frame_size;
    std::int32_t remaining_bytes = size;
    std::int32_t size_of_current_frame = 0;
    int number_of_frames = (size / frame_size) + 1;

    // this loop will send large messages in manageable frames.
    while(true)
    {
        frame++;

        // calculate the size of this frame
        if(remaining_bytes > frame_size)
        {
            remaining_bytes -= frame_size;
            size_of_current_frame = frame_size;
        }
        else
        {
            size_of_current_frame = remaining_bytes;
        }

        // configure the message header
        m_write_header.instID = 0; // not used yet
        m_write_header.frame_size = size_of_current_frame;
        m_write_header.max_size = size;
        m_write_header.type = EMsgTypData;
        m_write_header.frame_no = frame;
        m_write_header.max_frame_no = number_of_frames;

        m_write_data_buffer = {}; ///\ todo needed?
        m_write_frame_buffer.resize(m_write_header.frame_size + m_sizeOfHeader);
        std::memcpy(&m_write_frame_buffer[0], &m_write_header, m_sizeOfHeader);
        std::memcpy(&m_write_frame_buffer[m_sizeOfHeader], data, m_write_header.frame_size);

        m_write_data_buffer.payload.insert(m_write_data_buffer.payload.end(), m_write_frame_buffer.begin(), m_write_frame_buffer.end());

        m_writeQueueProtect.lock();
        m_write_queue.push(m_write_data_buffer);
        m_writeQueueProtect.unlock();

        if(m_write_header.frame_no == m_write_header.max_frame_no)
            break;

    } // while

    return true;
}

bool CTCPIPHelper::listenForData(const int fd)
{
    std::int32_t numOfBytesRead = 0;
    m_read_frame_buffer={};
    m_read_data_buffer={};

    // loop until all frames have been read
    while(!m_shutdownRequest)
    {
        // Check the contents of the header
        m_read_header = {};
        numOfBytesRead = read( fd , &m_read_header, m_sizeOfHeader);
        //numOfBytesRead = recv( fd , &m_read_header, m_sizeOfHeader, MSG_EOR|MSG_NOSIGNAL);
        //numOfBytesRead = recv( fd , &m_read_header, m_sizeOfHeader, 0);
        if(numOfBytesRead <= 0)
        {
            if(0 == numOfBytesRead)
            {
                CLOG(LOGLEV_RUN, "Connection closed,", ERR_STR);
                return false;
            }
            else if(-1 == numOfBytesRead)
            {
                CLOG(LOGLEV_RUN, "Error.", ERR_STR);
                return false;
            }
        }

        // check the message type
        if(EMsgTypData != m_read_header.type) {
            CLOG(LOGLEV_RUN, "wrong header type");
            return false;
        }

        m_read_frame_buffer = {};
        m_read_frame_buffer.resize(m_read_header.frame_size);
        numOfBytesRead = read(fd, m_read_frame_buffer.data(), m_read_header.frame_size);
        if(numOfBytesRead != static_cast<std::int32_t>(m_read_header.frame_size))
        {
            if(numOfBytesRead == -1)
            {
                CLOG(LOGLEV_RUN, "read error", ERR_STR);
                return false;
            }
            else if(numOfBytesRead == 0)
            {
                CLOG(LOGLEV_RUN, "Connection closed on other side", ERR_STR);
                return false;
            }
            else
            {
                CLOG(LOGLEV_RUN, "The number of bytes read "
                                , static_cast<std::int32_t>(m_read_header.frame_no)
                                , "/"
                                , static_cast<std::int32_t>(m_read_header.max_frame_no)
                                , " (", numOfBytesRead
                                ,") was not the expected amount (", m_read_header.frame_size, ")");
                return false;
            }
        }
        else
        {
            if(m_read_header.frame_no < m_read_header.max_frame_no)
            {
                // multi frame messages
                m_read_data_buffer.payload.insert(m_read_data_buffer.payload.end(), m_read_frame_buffer.begin(), m_read_frame_buffer.end());
            }
            else if(m_read_header.frame_no == m_read_header.max_frame_no)
            {
                // write the last part for multi-frame messages
                // or the complete message for single frame messages
                m_read_data_buffer.payload.insert(m_read_data_buffer.payload.end(), m_read_frame_buffer.begin(), m_read_frame_buffer.end());

                // check the expected size
                if(m_read_header.max_size == m_read_data_buffer.payload.size())
                {
                    m_readQueueProtect.lock();
                    m_read_queue.push(m_read_data_buffer);
                    m_readQueueProtect.unlock();
                    break; // we are done! leave the loop
                }
                else
                {
                    CLOG(LOGLEV_RUN, "The assembled message size (", m_read_data_buffer.payload.size()
                                    ,") was not the expected amount (", m_read_header.max_size, ")");

                }
            }
        }
    }

    return true;
}

bool CTCPIPHelper::writeData(const int fd)
{
    std::vector<char> data;
    int size;
    std::int32_t numOfBytesSent = 0;

    while(!m_shutdownRequest)
    {
        if(0U == m_write_queue.size()) {
            break;
        }

        m_writeQueueProtect.lock();
        //data = std::move(m_write_queue.front().payload);
        data = m_write_queue.front().payload;
        m_write_queue.pop();
        m_writeQueueProtect.unlock();
        size = data.size();

        //numOfBytesSent = send(fd, &data[0], size , MSG_EOR|MSG_NOSIGNAL );
        numOfBytesSent = send(fd, &data[0], size , 0 );
        if(numOfBytesSent < 0)
        {
            CLOG(LOGLEV_RUN, "error", ERR_STR);
            return false;
        }
        else
        {
            if(numOfBytesSent != static_cast<std::int32_t>(size))
            {
                CLOG(LOGLEV_RUN, "Number of bytes sent was not expected", ERR_STR);
                return false;
            }
        }
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

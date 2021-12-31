/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "posix_mq_helper.h"

#include "Logger.h"

#include <cstring>
#include <thread>
#include <cstring>

namespace comms {
namespace posix {
namespace helper {

CPOSIXMQHelper::CPOSIXMQHelper()
{
    m_sizeOfHeader = sizeof(SMessageHeader);
}

bool CPOSIXMQHelper::channel_create(std::string name)
{
    // create the posix mq channel
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = posix_conf::max_msg;
    attr.mq_msgsize = posix_conf::msg_size;
    attr.mq_curmsgs = 0;

    //mode_t mode  = S_IRWXU | S_IRWXG | S_IRWXO;
    int o_flag = O_CREAT | O_RDWR  ;//| O_NONBLOCK ;

    m_provider_channel_name = l_mq_channel_name + name;
    m_provider_channel_desc = mq_open(m_provider_channel_name.data(), o_flag, posix_conf::permission, &attr);

    if(-1 != m_provider_channel_desc) {
        CLOG(LOGLEV_RUN, "channel ", m_provider_channel_name, " created");

    } else {
        CLOG(LOGLEV_RUN, "open failed: (", errno, ") ", strerror(errno));
        return false;
    }

    return true;
}

bool CPOSIXMQHelper::cclient_connect(std::string channel)
{
    // ### Connect to remote

    for(int retry_index = 0; retry_index < l_max_num_of_connect_attempts; ++retry_index)
    {
        CLOG(LOGLEV_RUN, "channel ", channel, " connect attempt = ", retry_index + 1);
        if(l_max_num_of_connect_attempts == retry_index + 1) {
            CLOG(LOGLEV_RUN, "max channel connect attempts reached");
            return false;
        }

        // open as write only
        m_listener_channel_desc = mq_open(channel.data(), O_WRONLY);

        if(-1 != m_listener_channel_desc) {
            // no issues, this loop can be exited
            break;
        } else {
            CLOG(LOGLEV_RUN, "open failed = ", errno, " = ", strerror(errno));
        }

        // pause before the next attempt
        std::this_thread::sleep_for( std::chrono::milliseconds(200) );
    }

    CLOG(LOGLEV_RUN, "connected to channel ", channel);

    return true;
}

bool CPOSIXMQHelper::cclient_disconnect()
{
    if(0 != mq_close(m_listener_channel_desc)){
        CLOG(LOGLEV_RUN, "mq_close error on ", m_listener_channel_desc, ERR_STR);
        return false;
    }
    if(0 != mq_unlink(m_provider_channel_name.data())) {
        CLOG(LOGLEV_RUN, "mq_unlink error on ", m_provider_channel_name, ERR_STR);
        return false;
    }
    return true;
}

bool CPOSIXMQHelper::crecieve(std::vector<char>& data, int& size)
{
    // spin counter
    const int max_spin_count = 15;
    for(int index=0; index < max_spin_count; ++index)
    {
        if(m_read_queue.size() == 0)
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

    // read the buffered data
    m_recProtect.lock();
    data = m_read_queue.front().payload;
    m_read_queue.pop();
    m_recProtect.unlock();
    size = data.size();

    if(size <= 0)
        return false;

    return true;
}

bool CPOSIXMQHelper::ctransmit(mqd_t queue, const char *data, const int size)
{
    unsigned int priority = 1;

    m_write_header.max_size = size;
    m_write_header.type = EMsgTypData;
    m_transmitPackage.resize(size + m_sizeOfHeader);
    std::memcpy(&m_transmitPackage[0], &m_write_header, m_sizeOfHeader);
    std::memcpy(&m_transmitPackage[m_sizeOfHeader], data, size);

    int result = mq_send(queue, &m_transmitPackage[0], m_transmitPackage.size(), priority);
    if(result < 0) {
        CLOG(LOGLEV_RUN, "send failed = ", errno, " = ", strerror(errno));
        return false;
    } else {
        return true;
    }
}

bool CPOSIXMQHelper::listenForData(const mqd_t queue)
{
    std::uint32_t numOfBytesRead;

    // blocking read
    numOfBytesRead = mq_receive(queue, &m_rcvmsg[0], 1024, NULL);

    if(numOfBytesRead <= 0) {
        CLOG(LOGLEV_RUN, "Header read failed: ", errno, " - ", strerror(errno));
        return false;
    }

    SMessageHeader *pHeader = (SMessageHeader*)&m_rcvmsg[0];
    if(numOfBytesRead != (pHeader->max_size + m_sizeOfHeader))
    {
        CLOG(LOGLEV_RUN, "Header size mismatch: (", errno, ") ", strerror(errno));
        return false;
    }

    // check the data type
    if(EMsgTypData != pHeader->type)
    {
        CLOG(LOGLEV_RUN, "wrong header type");
        return false;
    }

    m_read_data_buffer.payload.resize(pHeader->max_size);
    std::memcpy(&m_read_data_buffer.payload[0], &m_rcvmsg[m_sizeOfHeader], pHeader->max_size);

    m_recProtect.lock();
    m_read_queue.push(m_read_data_buffer);
    m_recProtect.unlock();

    return true;
}

} // helper
} // posix
} // comms

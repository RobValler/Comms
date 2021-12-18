/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "posix_mq_helper.h"

#include "Logger.h"

#include <cstring>
#include <thread>

namespace comms {
namespace posix {
namespace helper {

bool CPOSIXMQHelper::crecieve(std::vector<char>& data, int& size)
{
    // spin counter
    const int max_spin_count = 15;
    for(int index=0; index < max_spin_count; index++)
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

    SReadBufferQ tmp;
    m_recProtect.lock();
    tmp = m_read_queue.front();
    m_read_queue.pop();
    m_recProtect.unlock();
    data = tmp.payload;
    size = tmp.payload.size();

    if(size <= 0)
        return false;

    return true;
}

bool CPOSIXMQHelper::ctransmit(mqd_t, const char *data, const int size)
{
    SMessageHeader head;
    head.size = size;
    head.type = EMsgTypData;

    m_transmitPackage.resize(size + m_sizeOfHeader);
    std::memcpy(&m_transmitPackage[0], &head, m_sizeOfHeader);
    std::memcpy(&m_transmitPackage[m_sizeOfHeader], data, size);

    unsigned int priority = 1;
    //if(0 < send(fd, &m_transmitPackage[0], m_transmitPackage.size() , 0 ))

    mqd_t queue = mq_open(l_channel_name, O_WRONLY);
    int result = mq_send(queue, &m_transmitPackage[0], m_transmitPackage.size(), priority);
    if(result < 0) {
        CLOG(LOGLEV_RUN, "send failed = ", errno, " = ", strerror(errno));
        return false;
    } else {
        return true;
    }
}

bool CPOSIXMQHelper::listenForData(const mqd_t)
{
    SMessageHeader peekHeader{};
    const int sizeOfHeader = sizeof(SMessageHeader);
    std::uint32_t numOfBytesRead;
    //int peekFlags = MSG_PEEK;

    // Check the contents of the header
   // numOfBytesRead = mq_receive(m_mqdes, (char*)&peekHeader, m_sizeOfHeader, NULL);

    char rcvmsg[50];
    mqd_t queue = mq_open(l_channel_name, O_RDONLY);
    numOfBytesRead = mq_receive(queue, rcvmsg, 50, NULL);

//    char rcvmsg[sizeOfHeader];
//    numOfBytesRead = mq_receive(m_mqdes, rcvmsg, sizeOfHeader, NULL);

    if(numOfBytesRead <= 0) {
        CLOG(LOGLEV_RUN, "Header read failed: ", errno, " - ", strerror(errno));
        return false;
    } else if(numOfBytesRead != static_cast<std::uint32_t>(sizeOfHeader)) {
        CLOG(LOGLEV_RUN, "Header size mismatch: (", errno, ") ", strerror(errno));
        return false;
    }

    // check the data type
    if(EMsgTypData != peekHeader.type) {
        CLOG(LOGLEV_RUN, "wrong header type");
        return false;
    }

    // store the actual data
    SReadBufferQ localReadBuffer;
    localReadBuffer.payload.resize(peekHeader.size);
    //numOfBytesRead = read(fd, localReadBuffer.payload.data(), localReadBuffer.payload.size());
    numOfBytesRead = mq_receive(queue, (char*)&localReadBuffer, peekHeader.size, NULL);
    if(numOfBytesRead != peekHeader.size)
    {
        CLOG(LOGLEV_RUN, "read size did not match");
        return false;
    }
    else
    {
        m_recProtect.lock();
        m_read_queue.push(localReadBuffer);
        m_recProtect.unlock();
        //CLOG(LOGLEV_RUN, "message recieved of ", numOfBytesRead, " bytes");
    }

    return true;
}

} // helper
} // posix
} // comms

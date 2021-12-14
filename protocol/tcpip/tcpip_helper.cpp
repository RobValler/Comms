
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
namespace common {

CTCPIPHelper::CTCPIPHelper()
{
    m_sizeOfHeader = sizeof(SMessageHeader);
}

bool CTCPIPHelper::crecieve(std::vector<char>& data, int& size)
{
    // spin counter
    const int max_spin_count = 15;
    for(int index=0; index < max_spin_count; index++)
    {
        if(m_read_queue.size() == 0)
        {
            if(index < max_spin_count - 2)
            {
                std::this_thread::sleep_for(std::chrono::nanoseconds(1));
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

bool CTCPIPHelper::ctransmit(const int fd, const char *data, const int size)
{
    SMessageHeader head;
    head.size = size;
    head.type = EMsgTypData;

    std::vector<char> package;
    package.resize(size + m_sizeOfHeader);
    std::memcpy(&package[0], &head, m_sizeOfHeader);
    std::memcpy(&package[m_sizeOfHeader], data, size);

    ssize_t result = send(fd , &package[0], package.size() , 0 );
    if(result > 0)
        return true;
    else
        return false;
}

bool CTCPIPHelper::listenForData(const int fd)
{
    SMessageHeader peekHeader;
    std::uint32_t numOfBytesRead;
    //int peekFlags = MSG_PEEK;

    // Check the contents of the header
    //numOfBytesRead = recv( fd , &peekHeader, m_sizeOfHeader, peekFlags);
    numOfBytesRead = read( fd , &peekHeader, m_sizeOfHeader);
    if(numOfBytesRead <= 0) {
        CLOG(LOGLEV_RUN, "Header read failed, numOfBytesRead = ", numOfBytesRead);
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
    numOfBytesRead = read(fd, localReadBuffer.payload.data(), localReadBuffer.payload.size());
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

int CTCPIPHelper::csizeOfReadBuffer()
{
    return m_read_queue.size();
}

} // comms
} // tcpip
} // common

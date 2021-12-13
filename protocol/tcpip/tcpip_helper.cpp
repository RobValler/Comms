
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

namespace comms {
namespace tcpip {
namespace common {

CTCPIPHelper::CTCPIPHelper()
{
    m_sizeOfHeader = sizeof(SMessageHeader);
}


bool CTCPIPHelper::crecieve(std::vector<char>& data, int& size)
{
    //size = m_size;
    //*data = &m_buffer[0];

    SReadBufferQ tmp;
    m_recProtect.lock();
    tmp = m_read_queue.back();
    m_read_queue.pop();
    m_recProtect.unlock();
    data = tmp.data;
    size = tmp.data.size();


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
    int numOfBytesRead;
    int sizeOfHeader = m_sizeOfHeader;
    int peekFlags = 0;
    peekFlags |= MSG_PEEK;

    // Check the contents of the header
    //numOfBytesRead = recv( m_connection_fd , &peekHeader, sizeOfHeader, peekFlags);
    numOfBytesRead = read( fd , &peekHeader, sizeOfHeader);
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
    int numOfBytesThatShouldBeRead = peekHeader.size;
    numOfBytesRead = read(fd , m_buffer, numOfBytesThatShouldBeRead);
    if(numOfBytesRead != numOfBytesThatShouldBeRead) {
        CLOG(LOGLEV_RUN, "read size did not match");
        return false;
    } else {
        m_size = numOfBytesRead;
        SReadBufferQ tmp;
        tmp.data.assign(m_buffer, m_buffer+m_size);
        m_recProtect.lock();
        m_read_queue.push(tmp);
        m_recProtect.unlock();
        CLOG(LOGLEV_RUN, "message recieved of ", numOfBytesRead, " bytes");
    }

    return true;
}

} // comms
} // tcpip
} // common

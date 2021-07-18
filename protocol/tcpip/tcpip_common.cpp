
#include "tcpip_common.h"

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

CTCPIP_Common::CTCPIP_Common()
{
    m_sizeOfHeader = sizeof(SMessageHeader);
}


bool CTCPIP_Common::crecieve(char** data, int& size)
{
    size = m_size;
    *data = &m_buffer[m_sizeOfHeader];

    if(size <= 0)
        return false;

    return true;
}

bool CTCPIP_Common::ctransmit(const char *data, const int size)
{
    SMessageHeader head;
    head.size = size;
    head.type = EMsgTypData;

    std::vector<char> package;
    package.resize(size + m_sizeOfHeader);
    std::memcpy(&package[0], &head, m_sizeOfHeader);
    std::memcpy(&package[m_sizeOfHeader], data, size);

    ssize_t result = send(m_connection_fd , &package[0], package.size() , 0 );
    if(result > 0)
        return true;
    else
        return false;
}

bool CTCPIP_Common::listenForData()
{
    SMessageHeader peekHeader;
    int numOfBytesRead;
    int sizeOfHeader = m_sizeOfHeader;
    int peekFlags = 0;
    peekFlags |= MSG_PEEK;

    // Check the contents of the header
    numOfBytesRead = recv( m_connection_fd , &peekHeader, sizeOfHeader, peekFlags);
    CLOG(LOGLEV_RUN, "header read failed");
    if(numOfBytesRead <= 0) {
        CLOG(LOGLEV_RUN, "numOfBytesRead = ", numOfBytesRead);
        return false;
    }

    // check the data type
    if(EMsgTypData != peekHeader.type) {
        CLOG(LOGLEV_RUN, "wrong type");
        return false;
    }

    // store the actual data
    int numOfBytesThatShouldBeRead = peekHeader.size + sizeOfHeader;
    numOfBytesRead = read(m_connection_fd , m_buffer, numOfBytesThatShouldBeRead);
    if(numOfBytesRead != numOfBytesThatShouldBeRead) {
        CLOG(LOGLEV_RUN, "read size did not match");
        return false;
    } else {
        m_size = numOfBytesRead;
        CLOG(LOGLEV_RUN, "message recieved of ", numOfBytesRead, " bytes");
    }

    return true;
}

} // comms
} // tcpip
} // common

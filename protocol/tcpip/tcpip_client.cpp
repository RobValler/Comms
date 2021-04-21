/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_client.h"

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
namespace client {

namespace {
    const int l_numOfConnectAttempts = 10;
}

enum EMessageType : std::uint8_t
{
    EMsgTypNone = 0,
    EMsgTypCtrl,
    EMsgTypData
};

struct SMessageHeader {
    std::uint32_t size;
    std::uint8_t type;
};

CTCPIPClient::CTCPIPClient()
    : m_shutdownrequest(false)
    , m_client_fd(0)
    , m_size(0)
{
    m_sizeOfHeader = sizeof(SMessageHeader);
}

CTCPIPClient::~CTCPIPClient()
{
    m_shutdownrequest = true;
    shutdown(m_client_fd, SHUT_RDWR); // aborts any blocking calls on the client

    if(t_client.joinable())
        t_client.join();
    else
        CLOG(LOGLEV_RUN, "client join issue");
}

bool CTCPIPClient::client_connect(std::string ip_address)
{
    long socket_args = 0;
    char confirmMsgBuff[48] = {0};
    struct sockaddr_in serv_addr;

    if ((m_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        CLOG(LOGLEV_RUN, "socket failure");
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if(inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr) <= 0)
        return false;

    socket_args |= O_NONBLOCK;
    if( fcntl(m_client_fd, F_SETFL, socket_args) < 0) {
        CLOG(LOGLEV_RUN, "set socket arguments failed");
       return false;
    }

    // attempt connect, multiple tries
    for(int retry_index = 0; retry_index < l_numOfConnectAttempts + 1; retry_index++)
    {
        if(l_numOfConnectAttempts == retry_index) {
            CLOG(LOGLEV_RUN, "max number of connect attemps expired");
            return false;
        } else {
            CLOG(LOGLEV_RUN, "Attempt number = ", retry_index + 1);
            if (connect(m_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                CLOG(LOGLEV_RUN, "connection failed");
            } else {
                CLOG(LOGLEV_RUN, "client connected");
                break;
            }
        }
        std::this_thread::sleep_for( std::chrono::milliseconds(200) );
    }

    SMessageHeader head;
    if( read(m_client_fd , confirmMsgBuff, 1024) <= 0) {
        CLOG(LOGLEV_RUN, "confirm message read failed");
        return false;
    }

    std::memcpy(&head, &confirmMsgBuff[0], m_sizeOfHeader);
    if(EMsgTypCtrl != head.type) {
        CLOG(LOGLEV_RUN, "wrong msg type");
        return false;
    }

    CLOG(LOGLEV_RUN, "client connection confirmed");

    // client is connected. Start the client data thread
    t_client = std::thread(&CTCPIPClient::threadfunc_client, this);

    return true;
}

bool CTCPIPClient::client_disconnect()
{
    return false;
}

bool CTCPIPClient::recieve(char** data, int& size)
{
    size = m_size;
    *data = &m_buffer[m_sizeOfHeader];

    if(size <= 0)
        return false;

    return true;
}

bool CTCPIPClient::transmit(const char *data, const int size)
{
    SMessageHeader head;
    head.size = size;
    head.type = EMsgTypData;

    std::vector<char> package;
    package.resize(size + m_sizeOfHeader);
    std::memcpy(&package[0], &head, m_sizeOfHeader);
    std::memcpy(&package[m_sizeOfHeader], data, size);

    ssize_t result = send(m_client_fd , &package[0], package.size() , 0 );
    if(result > 0)
        return true;
    else
        return false;
}

void CTCPIPClient::threadfunc_client()
{
    while(!m_shutdownrequest)
    {
        // listenForData() is a blocking read so we dont need a thread throttle
        if(!listenForData()) {
            CLOG(LOGLEV_RUN, "error on listen");
            break;
        }
    }
}

bool CTCPIPClient::listenForData()
{
    SMessageHeader peekHeader;
    int numOfBytesRead;
    int sizeOfHeader = m_sizeOfHeader;
    int peekFlags = 0;
    peekFlags |= MSG_PEEK;

    // Check the contents of the header
    numOfBytesRead = recv( m_client_fd , &peekHeader, sizeOfHeader, peekFlags);
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
    numOfBytesRead = read(m_client_fd , m_buffer, numOfBytesThatShouldBeRead);
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
} // posix
} // client

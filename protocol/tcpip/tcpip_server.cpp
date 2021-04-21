/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_server.h"

#include "Logger.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

namespace comms {
namespace tcpip {
namespace server {

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

CTCPIPServer::CTCPIPServer()
    : m_shutdownrequest(false)
    , m_serverSocket(0)
    , m_server_fd(0)
    , m_size(0)
{
    m_sizeOfHeader = sizeof(SMessageHeader);
    t_server = std::thread(&CTCPIPServer::threadfunc_server, this);
}

CTCPIPServer::~CTCPIPServer()
{
    m_shutdownrequest = true;
    shutdown(m_server_fd, SHUT_RDWR); // aborts any blocking calls on the server

    if(t_server.joinable())
        t_server.join();
    else
        CLOG(LOGLEV_RUN, "server join issue");
}

bool CTCPIPServer::server_connect()
{
    int opt = 1;
    struct sockaddr_in address;    
    int addrlen = sizeof(address);

    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        CLOG(LOGLEV_RUN, "socket returned an error");
        return false;
    }

    if (setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        CLOG(LOGLEV_RUN, "setsockopt failed");
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( 8080 );

    if (bind(m_server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        CLOG(LOGLEV_RUN, "bind failed");
        return false;
    }

    if (listen(m_server_fd, 10) < 0) {
        CLOG(LOGLEV_RUN, "listen failed");
        return false;
    }

    //set non blocking
//    if(fcntl(m_server_fd, F_SETFL, fcntl(m_server_fd, F_GETFL) | O_NONBLOCK) < 0) {
//        CLOG(LOGLEV_RUN, " fcntl failed");
//        return false;
//    }


    m_serverSocket = accept(m_server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if ( m_serverSocket < 0) {
        if(!m_shutdownrequest){
            CLOG(LOGLEV_RUN, "accept failed = ", m_serverSocket);
            return false;
        } else {
            return true;
        }
    }

    CLOG(LOGLEV_RUN, "server connected");

    // send the confirmation message
    char confirmMsgBuff;
    SMessageHeader head;
    head.size = 1;
    head.type = EMsgTypCtrl;

    std::vector<char> package;
    package.resize(1 + m_sizeOfHeader);
    std::memcpy(&package[0], &head, m_sizeOfHeader);
    std::memcpy(&package[m_sizeOfHeader], &confirmMsgBuff, 1);
    ssize_t result = send(m_serverSocket , &package[0] , package.size() , 0 );
    if(result <= 0) {
        CLOG(LOGLEV_RUN, "send fail");
        return false;
    }

    return true;
}

bool CTCPIPServer::recieve(char** data, int& size)
{
    size = m_size;
    *data = &m_buffer[m_sizeOfHeader];

    if(size <= 0)
        return false;

    return true;
}

bool CTCPIPServer::transmit(const char *data, const int size)
{
    SMessageHeader head;
    head.size = size;
    head.type = EMsgTypData;

    std::vector<char> package;
    package.resize(size + m_sizeOfHeader);
    std::memcpy(&package[0], &head, m_sizeOfHeader);
    std::memcpy(&package[m_sizeOfHeader], data, size);

    ssize_t result = send(m_serverSocket , &package[0], package.size() , 0 );
    if(result > 0)
        return true;
    else
        return false;
}

void CTCPIPServer::threadfunc_server()
{
    bool result = false;
    result = server_connect();
    if(result)
        listenForData();
    else
        CLOG(LOGLEV_RUN, "server_connect failed");

}

bool CTCPIPServer::listenForData()
{
    SMessageHeader peekHeader;
    int numOfBytesRead;
    int sizeOfHeader = m_sizeOfHeader;
    int peekFlags = 0;
    peekFlags |= MSG_PEEK;

    // Check the contents of the header
    numOfBytesRead = recv( m_serverSocket , &peekHeader, sizeOfHeader, peekFlags);
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
    numOfBytesRead = read(m_serverSocket , m_buffer, numOfBytesThatShouldBeRead);
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
} // server

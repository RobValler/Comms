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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

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

}

CTCPIPClient::~CTCPIPClient()
{
    m_shutdownrequest = true;
    shutdown(m_client_fd, SHUT_RDWR); // aborts any blocking calls on the client

    if(t_client.joinable())
        t_client.join();
    else
        CLogger::Print(LOGLEV_RUN, "~CTCPIP()", "client join issue");
}

bool CTCPIPClient::client_connect()
{
    char confirmMsgBuff[48] = {0};
    struct sockaddr_in serv_addr;

    if ((m_client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        CLogger::Print(LOGLEV_RUN, "client_connect: ", "socket failure");
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
        return false;

    if (connect(m_client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        CLogger::Print(LOGLEV_RUN, "client_connect: ", "connection failed");
        return false;
    }

    CLogger::Print(LOGLEV_RUN, "client connected");

    SMessageHeader head;
    if( read(m_client_fd , confirmMsgBuff, 1024) <= 0) {
        CLogger::Print(LOGLEV_RUN, "client_connect: ", "confirm message read failed");
        return false;
    }

    std::memcpy(&head, &confirmMsgBuff[0], sizeof(SMessageHeader));
    if(EMsgTypCtrl != head.type) {
        CLogger::Print(LOGLEV_RUN, "client_connect: ", "wrong msg type");
        return false;
    }

    CLogger::Print(LOGLEV_RUN, "client connection confirmed");

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
    *data = &m_buffer[sizeof(SMessageHeader)];

    if(size <= 0)
        return false;

    return true;
}

bool CTCPIPClient::transmit(const char *data, const int size)
{
    SMessageHeader head;
    head.size = size;
    head.type = EMsgTypData;
    char package[size + sizeof(SMessageHeader)]; // todo: change to dynamic array
    std::memcpy(&package[0], &head, sizeof(SMessageHeader));
    std::memcpy(&package[sizeof(SMessageHeader)], data, size);

    ssize_t result = send(m_client_fd , package , sizeof(package) , 0 );
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
            CLogger::Print(LOGLEV_RUN, "threadfunc_client: ", "error on listen");
            break;
        }
    }
}

bool CTCPIPClient::listenForData()
{
    SMessageHeader peekHeader;
    int numOfBytesRead;
    int sizeOfHeader = sizeof(peekHeader);
    int peekFlags = 0;
    peekFlags |= MSG_PEEK;

    // Check the contents of the header
    numOfBytesRead = recv( m_client_fd , &peekHeader, sizeOfHeader, peekFlags);
    CLogger::Print(LOGLEV_RUN, "recieve: ", "header read failed");
    if(numOfBytesRead <= 0) {
        CLogger::Print(LOGLEV_RUN, "recieve: ", "numOfBytesRead = ", numOfBytesRead);
        return false;
    }

    // check the data type
    if(EMsgTypData != peekHeader.type) {
        CLogger::Print(LOGLEV_RUN, "recieve() ", "wrong type");
        return false;
    }

    // store the actual data
    int numOfBytesThatShouldBeRead = peekHeader.size + sizeOfHeader;
    numOfBytesRead = read(m_client_fd , m_buffer, numOfBytesThatShouldBeRead);
    if(numOfBytesRead != numOfBytesThatShouldBeRead) {
        CLogger::Print(LOGLEV_RUN, "recieve: ", "read size did not match");
        return false;
    } else {
        m_size = numOfBytesRead;
        CLogger::Print(LOGLEV_RUN, "message recieved of ", numOfBytesRead, " bytes");
    }

    return true;
}

/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip.h"

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

CTCPIP::CTCPIP()
    : m_shutdownrequest(false)
{
    t_server = std::thread(&CTCPIP::threadfunc_server, this);
    //t_client = std::thread(&CTCPIP::threadfunc_client, this);
}

CTCPIP::~CTCPIP()
{
    m_shutdownrequest = true;
    shutdown(m_server_fd, SHUT_RDWR); // aborts any blocking calls
    t_server.join();
    //t_client.join();
}

bool CTCPIP::server_connect()
{
    int valread;
    int opt = 1;
    struct sockaddr_in address;    
    int addrlen = sizeof(address);

    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        CLogger::Print(LOGLEV_RUN, "server_connect", "socket");
        return false;
    }

    if (setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        CLogger::Print(LOGLEV_RUN, "server_connect", "setsockopt");
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( 8080 );

    if (bind(m_server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        CLogger::Print(LOGLEV_RUN, "server_connect", "bind");
        return false;
    }

    if (listen(m_server_fd, 10) < 0) {
        CLogger::Print(LOGLEV_RUN, "server_connect", "listen");
        return false;
    }

    //set non blocking
//    if(fcntl(m_server_fd, F_SETFL, fcntl(m_server_fd, F_GETFL) | O_NONBLOCK) < 0) {
//        CLogger::Print(LOGLEV_RUN, "server_connect", " fcntl failed");
//        return false;
//    }


    m_serverSocket = accept(m_server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if ( m_serverSocket < 0) {
        if(!m_shutdownrequest){
            CLogger::Print(LOGLEV_RUN, "server_connect", " accept failed = ", m_serverSocket);
            return false;
        } else {
            return true;
        }
    }

    CLogger::Print(LOGLEV_RUN, "server connected");

    // send the confirmation message
    char confirmMsgBuff;
    SMessageHeader head;
    head.size = 1;
    head.type = EMsgTypCtrl;
    char package[1 + sizeof(SMessageHeader)];
    std::memcpy(&package[0], &head, sizeof(SMessageHeader));
    std::memcpy(&package[sizeof(SMessageHeader)], &confirmMsgBuff, 1);
    ssize_t result = send(m_serverSocket , package , sizeof(package) , 0 );
    if(result <= 0) {
        CLogger::Print(LOGLEV_RUN, "server_connect", "send");
        return false;
    }

    // loop read
//    while(!m_shutdownrequest)
//    {
//        valread = recv( m_serverSocket , m_buffer, 1024, 0);
        //valread = read(m_serverSocket , m_buffer, 1024);
//        CLogger::Print(LOGLEV_RUN, "message recieved of ", valread, " bytes");
//    }

    return true;
}

bool CTCPIP::client_connect()
{
    char confirmMsgBuff[48] = {0};
    struct sockaddr_in serv_addr;

    if ((m_clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        CLogger::Print(LOGLEV_RUN, "client_connect", "socket");
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
        return false;

    if (connect(m_clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        CLogger::Print(LOGLEV_RUN, "client_connect", "connect");
        return false;
    }

    CLogger::Print(LOGLEV_RUN, "client connected");

    SMessageHeader head;
    if( read(m_clientSocket , confirmMsgBuff, 1024) <= 0) {
        CLogger::Print(LOGLEV_RUN, "client_connect", "socket");
        return false;
    }

    std::memcpy(&head, &confirmMsgBuff[0], sizeof(SMessageHeader));
    if(EMsgTypCtrl != head.type) {
        CLogger::Print(LOGLEV_RUN, "client_connect", "wrong type");
        return false;
    }

    CLogger::Print(LOGLEV_RUN, "client connection confirmed");

    return true;
}

bool CTCPIP::disconnect()
{
    return false;
}

bool CTCPIP::recieve(char** data, int& size)
{
    SMessageHeader head;

//    int numOfBytesRead = recv( m_serverSocket , m_buffer, 1024, 0);
    int numOfBytesRead = read(m_serverSocket , m_buffer, 1024);
    CLogger::Print(LOGLEV_RUN, "message recieved of ", numOfBytesRead, " bytes");

    std::memcpy(&head, &m_buffer[0], sizeof(SMessageHeader));
    if(EMsgTypData != head.type) {
        CLogger::Print(LOGLEV_RUN, "recieve() ", "wrong type");
        return false;
    }

    size = head.size;
    *data = &m_buffer[sizeof(SMessageHeader)];

    if(size <= 0)
        return false;

    return true;
}

bool CTCPIP::transmit(const char *data, const int size)
{
    SMessageHeader head;
    head.size = size;
    head.type = EMsgTypData;
    char package[size + sizeof(SMessageHeader)];
    std::memcpy(&package[0], &head, sizeof(SMessageHeader));
    std::memcpy(&package[sizeof(SMessageHeader)], data, size);

    ssize_t result = send(m_clientSocket , package , sizeof(package) , 0 );
    if(result > 0)
        return true;
    else
        return false;
}

void CTCPIP::threadfunc_server()
{
    server_connect();
}

void CTCPIP::threadfunc_client()
{

}

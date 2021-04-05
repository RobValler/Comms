/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>






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

    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        return false;

    if (setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        return false;

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( 8080 );

    if (bind(m_server_fd, (struct sockaddr *)&address, sizeof(address))<0)
        return false;

    if (listen(m_server_fd, 10) < 0)
        return false;

//    if(fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL) | O_NONBLOCK) < 0)
//        return false;

    if ((m_serverSocket = accept(m_server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
        return false;

    std::cout << "server_connect good" << std::endl;

    //valread = recv( m_serverSocket , buffer, 1024, 0);
    valread = read(m_serverSocket , m_buffer, 1024);

    std::cout << "message recieved = " << m_buffer << std::endl;

    return true;
}

bool CTCPIP::client_connect()
{
    struct sockaddr_in serv_addr;

    if ((m_clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return false;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
        return false;

    if (connect(m_clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        return false;

    std::cout << "client_connect good" << std::endl;

    return true;
}

bool CTCPIP::disconnect()
{
    return false;
}

bool CTCPIP::recieve(char** data, int& size)
{
    *data = &m_buffer[0];
    size = sizeof(m_buffer);
    return false;
}

bool CTCPIP::transmit(const char *data, const int size)
{
    ssize_t result = send(m_clientSocket , data , size , 0 );
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

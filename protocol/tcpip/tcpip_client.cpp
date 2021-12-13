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
    const int l_delayBetweenConnectAttempt_ms = 100;
}

CTCPIPClient::CTCPIPClient()
    : m_shutdownrequest(false)
{
    if(m_blocking){
        m_socket_type = SOCK_STREAM;
    }
    else
    {
        m_socket_type = SOCK_STREAM | SOCK_NONBLOCK;
    }
}

CTCPIPClient::~CTCPIPClient()
{
    m_shutdownrequest = true;
    shutdown(m_connection_fd, SHUT_RDWR); // aborts any blocking calls on the client

    if(t_client.joinable())
        t_client.join();
    else
        CLOG(LOGLEV_RUN, "client join issue");
}

bool CTCPIPClient::client_connect(std::string ip_address)
{
//    long socket_args = 0;
    char confirmMsgBuff[48] = {0};
    struct sockaddr_in serv_addr;

    if ((m_connection_fd = socket(AF_INET, m_socket_type, 0)) < 0) {
        CLOG(LOGLEV_RUN, "socket failure");
        return false;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    if(inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr) <= 0)
        return false;

//    socket_args |= O_NONBLOCK;
//    if( fcntl(m_connection_fd, F_SETFL, socket_args) < 0) {
//        CLOG(LOGLEV_RUN, "set socket arguments failed");
//       return false;
//    }

    // attempt connect, multiple tries
    for(int retry_index = 0; retry_index < l_numOfConnectAttempts + 1; retry_index++)
    {
        if(l_numOfConnectAttempts == retry_index) {
            CLOG(LOGLEV_RUN, "max number of connect attemps expired");
            return false;
        } else {
            CLOG(LOGLEV_RUN, "Attempt number = ", retry_index + 1);
            if (connect(m_connection_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
                // if the connection failed, do nothing and try again in the next loop.
                CLOG(LOGLEV_RUN, "connection failed");
            } else {
                // client is connected, exit the loop
                CLOG(LOGLEV_RUN, "client connected");
                break;
            }
        }
        std::this_thread::sleep_for( std::chrono::milliseconds(l_delayBetweenConnectAttempt_ms) );
    }

    common::SMessageHeader head;
    if( read(m_connection_fd , confirmMsgBuff, 1024) <= 0) {
        CLOG(LOGLEV_RUN, "confirm message read failed");
        return false;
    }

    std::memcpy(&head, &confirmMsgBuff[0], m_sizeOfHeader);
    if(common::EMsgTypCtrl != head.type) {
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
    ///\ todo fix!
    return false;
}

void CTCPIPClient::threadfunc_client()
{
    while(!m_shutdownrequest)
    {
        // listenForData() is a blocking read so we dont need a thread throttle
        if(!listenForData(m_connection_fd)) {
            CLOG(LOGLEV_RUN, "error on listen");
            break;
        }
    }
}

} // comms
} // posix
} // client

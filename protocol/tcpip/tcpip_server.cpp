/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
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

CTCPIPServer::CTCPIPServer()
    : m_shutdownrequest(false)
{
    if(m_blocking){
        m_socket_type = SOCK_STREAM;
    }
    else
    {
        m_socket_type = SOCK_STREAM | SOCK_NONBLOCK;
    }

    t_server = std::thread(&CTCPIPServer::threadfunc_server, this);
}

CTCPIPServer::~CTCPIPServer()
{
    m_shutdownrequest = true;
    shutdown(m_connection_fd, SHUT_RDWR); // aborts any blocking calls on the server

    if(t_server.joinable())
        t_server.detach(); ///\todo re-evaluate!
    else
        CLOG(LOGLEV_RUN, "server join issue");
}

bool CTCPIPServer::channel_create()
{
    int opt = 1;
    struct sockaddr_in address;    
    int addrlen = sizeof(address);

    if ((m_connection_fd = socket(AF_INET, m_socket_type, 0)) == 0) {
        CLOG(LOGLEV_RUN, "socket returned an error");
        return false;
    }

    if (setsockopt(m_connection_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        CLOG(LOGLEV_RUN, "setsockopt failed");
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( 8080 );

    if (bind(m_connection_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        CLOG(LOGLEV_RUN, "bind failed");
        return false;
    }

    // listen out for any external connections.
    if (listen(m_connection_fd, 10) < 0) {
        CLOG(LOGLEV_RUN, "listen failed");
        return false;
    }

    //set non blocking
//    if(fcntl(m_connection_fd, F_SETFL, fcntl(m_connection_fd, F_GETFL) | O_NONBLOCK) < 0) {
//        CLOG(LOGLEV_RUN, " fcntl failed");
//        return false;
//    }

    // accept the connection.
    m_connection_socket = accept(m_connection_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if ( m_connection_socket < 0) {
        if(!m_shutdownrequest) {
            CLOG(LOGLEV_RUN, "accept failed = ", m_connection_socket);
            return false;
        } else {
            return true;
        }
    }

    CLOG(LOGLEV_RUN, "server has connected to a client");

    // send the confirmation message
    SMessageHeader head;
    head.size = 0;
    head.type = EMsgTypCtrl;
    if(0 > send(m_connection_socket, &head, m_sizeOfHeader, 0 ))
    {
        CLOG(LOGLEV_RUN, "confirmation send failed");
        return false;
    }

    return true;
}

void CTCPIPServer::threadfunc_server()
{
    bool result = channel_create();
    if(result)
    {
        while(!m_shutdownrequest)
        {
            // listenForData() is a blocking read
            if(!listenForData(m_connection_socket))
            {
                CLOG(LOGLEV_RUN, "error on listen");
                break;
            }
        }
    }
    else
    {
        CLOG(LOGLEV_RUN, "server_connect failed");
    }
}

} // comms
} // tcpip
} // server

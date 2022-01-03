/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
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

}

CTCPIPServer::~CTCPIPServer()
{
    shutdown(m_connection_fd, SHUT_RDWR); // aborts any blocking calls on the server
    m_shutdownrequest = true;
}

bool CTCPIPServer::channel_create(std::string)
{
    if ((m_connection_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        CLOG(LOGLEV_RUN, "socket returned an error");
        return false;
    }

    int opt = 1;
    if (setsockopt(m_connection_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        CLOG(LOGLEV_RUN, "setsockopt failed");
        return false;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons( 8888 );

    if (bind(m_connection_fd, (struct sockaddr *)&server_address, sizeof(server_address))<0)
    {
        CLOG(LOGLEV_RUN, "bind failed", ERR_STR);
        return false;
    }

    // listen out for any external connections.
    if (listen(m_connection_fd, 3) < 0) {
        CLOG(LOGLEV_RUN, "listen failed");
        return false;
    }

    //set non blocking
//    if(fcntl(m_connection_fd, F_SETFL, fcntl(m_connection_fd, F_GETFL) | O_NONBLOCK) < 0) {
//        CLOG(LOGLEV_RUN, " fcntl failed");
//        return false;
//    }

    // accept the connection.
    struct sockaddr_in client_address;
    int addrlen_client = sizeof(struct sockaddr_in);
    m_connection_socket = accept(m_connection_fd, (struct sockaddr *)&client_address, (socklen_t*)&addrlen_client);
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
    head.instID = 0;
//    head.frame_size = 0;
    head.max_size = 0;
    head.type = EMsgTypCtrl;
    head.frame_no = 1;
    head.max_frame_no = 1;
    if(0 > send(m_connection_socket, &head, m_sizeOfHeader, 0 ))
    {
        CLOG(LOGLEV_RUN, "confirmation send failed");
        return false;
    }

    return true;
}

} // comms
} // tcpip
} // server

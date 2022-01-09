/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_client.h"

#include "Logger.h"
#include "common.h"

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
    const int l_delayBetweenConnectAttempt_ms = 1000;
}

CTCPIPClient::~CTCPIPClient()
{
    static_cast<void>(client_disconnect());
    m_shutdownrequest = true;
}

bool CTCPIPClient::client_connect(std::string)
{

    bool result = false;

    m_connection_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == m_connection_fd)
    {
        CLOG(LOGLEV_RUN, "socket failure", ERR_STR);
        return false;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8888);


//    if(inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr) <= 0)
//    {
//        CLOG(LOGLEV_RUN, "inet_pton failed", ERR_STR);
//        return false;
//    }

//    socket_args |= O_NONBLOCK;
//    if( fcntl(m_connection_fd, F_SETFL, socket_args) < 0) {
//        CLOG(LOGLEV_RUN, "set socket arguments failed");
//       return false;
//    }

    // attempt connect, multiple tries
    for(int retry_index = 0; retry_index < l_max_num_of_connect_attempts; ++retry_index)
    {
        if(l_max_num_of_connect_attempts == retry_index)
        {
            CLOG(LOGLEV_RUN, "max number of connect attemps expired");
            result = false;
            break;
        }
        else
        {
            CLOG(LOGLEV_RUN, "Attempt number ", retry_index + 1);
            if (0 > connect(m_connection_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
            {
                // if the connection failed, do nothing and try again in the next loop.
                CLOG(LOGLEV_RUN, "Connection attempt failed.", ERR_STR);
            }
            else
            {
                CLOG(LOGLEV_RUN, "Client connected.");

                // read handshaking message with server
                m_confirmMsgBuff.resize(m_sizeOfHeader);
                if(-1 == read(m_connection_fd , &m_confirmMsgBuff[0], m_sizeOfHeader))
                {
                    CLOG(LOGLEV_RUN, "confirmation message read failed");
                    continue;
                }

                // check the handshake message
                SMessageHeader *pHeader = (SMessageHeader*)&m_confirmMsgBuff[0];
                if(EMsgTypCtrl != pHeader->type)
                {
                    CLOG(LOGLEV_RUN, "Wrong msg type");
                    continue;
                }

                CLOG(LOGLEV_RUN, "Client connection confirmed");

                result = true;
                break;
            } //
        } //

        std::this_thread::sleep_for( std::chrono::milliseconds(l_delayBetweenConnectAttempt_ms) );

    } // for

    return result;
}

bool CTCPIPClient::client_disconnect()
{
     // aborts any blocking calls on the client
    if(0 == shutdown(m_connection_fd, SHUT_RDWR)) {
        return true;
    } else {
        CLOG(LOGLEV_RUN, "failed shutdown on ", m_connection_fd, ERR_STR);
        return false;
    }
}

} // comms
} // posix
} // client

/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
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

    static_cast<void>(client_disconnect());

    if(t_client.joinable())
        t_client.join();
    else
        CLOG(LOGLEV_RUN, "client join issue");
}

bool CTCPIPClient::client_connect(std::string ip_address)
{
    struct sockaddr_in serv_addr;
    bool result = false;

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
                CLOG(LOGLEV_RUN, "connection attempt failed");
                continue;
            }
            else
            {
                CLOG(LOGLEV_RUN, "client connected");

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
                    CLOG(LOGLEV_RUN, "wrong msg type");
                    continue;
                }

                CLOG(LOGLEV_RUN, "client connection confirmed");

                // client is connected. Start the client data thread
                t_client = std::thread(&CTCPIPClient::threadfunc_client, this);

                result = true;
                break;
            } // else
        } // if(l_max_num_of_connect_attempts
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

void CTCPIPClient::threadfunc_client()
{
    while(!m_shutdownrequest)
    {
        // listenForData() is a blocking read
        if(!listenForData(m_connection_fd))
        {
            CLOG(LOGLEV_RUN, "error on listen");
            break;
        }
    }
}

} // comms
} // posix
} // client

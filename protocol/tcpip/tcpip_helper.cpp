/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "tcpip_helper.h"
#include "common.h"
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
#include <signal.h>
#include <chrono>
#include <thread>

namespace comms {
namespace tcpip {
namespace helper {

/// \ brief interrupt handler
void my_handler(int)
{
    CLOG(LOGLEV_RUN, "--> SIG event triggered.", ERR_STR, "<--");

}

CTCPIPHelper::CTCPIPHelper()
{
    m_sizeOfHeader = sizeof(SMessageHeader);

    // interrupt handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    //sigaction(SIGINT,   &sigIntHandler, NULL); // CTRL+C
    sigaction(SIGPIPE,  &sigIntHandler, NULL); // broken pipe

}

CTCPIPHelper::~CTCPIPHelper()
{

}

bool CTCPIPHelper::crecieve(const int fd, std::vector<char>& data, int&)
{
    int numOfBytesRead = 0;

    // Check the contents of the header
    m_read_header = {};
    numOfBytesRead = read( fd , &m_read_header, m_sizeOfHeader);
    //numOfBytesRead = recv( fd , &m_read_header, m_sizeOfHeader, MSG_EOR|MSG_NOSIGNAL);
    //numOfBytesRead = recv( fd , &m_read_header, m_sizeOfHeader, MSG_PEEK);
    if(numOfBytesRead <= 0)
    {
        if(0 == numOfBytesRead)
        {
            CLOG(LOGLEV_RUN, "Connection closed,", ERR_STR);
            return false;
        }
        else if(-1 == numOfBytesRead)
        {
            CLOG(LOGLEV_RUN, "Error.", ERR_STR);
            return false;
        }
        else if(numOfBytesRead != m_sizeOfHeader)
        {
            CLOG(LOGLEV_RUN, "Header size mismatch.");
            return false;
        }
    }

    // check the message type
    if(EMsgTypData != m_read_header.type) {
        CLOG(LOGLEV_RUN, "wrong header type");
        return false;
    }

    data = {};
    data.resize(m_read_header.frame_size);
    numOfBytesRead = read(fd, data.data(), m_read_header.frame_size);
//    numOfBytesRead = recv(fd, data.data(), m_read_header.frame_size, 0);
    if(numOfBytesRead != static_cast<std::int32_t>(m_read_header.frame_size))
    {
        if(numOfBytesRead == -1)
        {
            CLOG(LOGLEV_RUN, "read error", ERR_STR);
            return false;
        }
        else if(numOfBytesRead == 0)
        {
            CLOG(LOGLEV_RUN, "Connection closed on other side", ERR_STR);
            return false;
        }
        else
        {
            CLOG(LOGLEV_RUN, "The number of bytes read "
                            , static_cast<std::int32_t>(m_read_header.frame_no)
                            , "/"
                            , static_cast<std::int32_t>(m_read_header.max_frame_no)
                            , " (", numOfBytesRead
                            ,") was not the expected amount (", m_read_header.frame_size, ")");
            return false;
        }
    }

    return true;
}

bool CTCPIPHelper::ctransmit(const int fd, const char *data, const int size)
{
    if(size > tcpip_conf::max_msg_size_allowable)
    {
        CLOG(LOGLEV_RUN, "This message is too large, aborted!");
        return false;
    }

    // configure the message header
    m_write_header.instID = 0; // not used yet
    m_write_header.frame_size = size;
    m_write_header.max_size = size;
    m_write_header.type = EMsgTypData;
    m_write_header.frame_no = 1;
    m_write_header.max_frame_no = 1;

    std::vector<char> m_write_data_buffer = {}; ///\ todo needed?
    m_write_data_buffer.resize(m_write_header.frame_size + m_sizeOfHeader);
    std::memcpy(&m_write_data_buffer[0], &m_write_header, m_sizeOfHeader);
    std::memcpy(&m_write_data_buffer[m_sizeOfHeader], data, m_write_header.frame_size);


    //numOfBytesSent = send(fd, &data[0], size , MSG_EOR|MSG_NOSIGNAL );
    int numOfBytesSent = send(fd, &m_write_data_buffer[0], m_write_data_buffer.size() , 0 );
    if(numOfBytesSent < 0)
    {
        CLOG(LOGLEV_RUN, "error", ERR_STR);
        return false;
    }
    else
    {
        if(numOfBytesSent != static_cast<std::int32_t>(m_write_data_buffer.size()))
        {
            CLOG(LOGLEV_RUN, "Number of bytes sent was not expected", ERR_STR);
            return false;
        }
    }


    return true;
}

} // helper
} // tcpip
} // comms

/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "posix_mq_client.h"

#include "Logger.h"

#include <vector>
#include <cstring>
#include <chrono>
#include <thread>

namespace comms {
namespace posix {
namespace client {

//
namespace  {
    const int l_max_num_of_connect_attempts = 5;
}

CPOSIXMQClient::CPOSIXMQClient()
    : m_shutdownrequest(false)
    , m_mqdes_client(-1)
{
    m_sizeOfHeader = sizeof(SMessageHeader);
//    t_client = std::thread(&CPOSIXMQClient::threadfunc_client, this);
}

CPOSIXMQClient::~CPOSIXMQClient()
{
    static_cast<void>(client_disconnect());
    m_shutdownrequest = true;
//    t_server.join();
//    t_client.join();
}

bool CPOSIXMQClient::client_connect(std::string channel)
{
    // create the posix mq channel
    // will attempt several times

    //disabled mq_attr because using it causes access permission errors
//    mq_attr attrib;
//    attrib.mq_flags = 0;
//    attrib.mq_maxmsg = 16;
//    attrib.mq_msgsize = 1024;
//    attrib.mq_curmsgs = 0;

    //mode_t mode  = S_IRWXU | S_IRWXG | S_IRWXO;
    int o_flag = /* O_CREAT |*/ O_RDWR ;// | O_NONBLOCK ;

    for(int retry_index = 0; retry_index < l_max_num_of_connect_attempts; retry_index++)
    {
        CLOG(LOGLEV_RUN, "channel connect attempt = ", retry_index + 1);
        if(l_max_num_of_connect_attempts == retry_index + 1) {
            CLOG(LOGLEV_RUN, "max channel connect attempts reached");
            return false;
        }

        m_mqdes_client = mq_open(channel.c_str(),  o_flag, 0666, NULL);
        if(-1 != m_mqdes_client) {
            // no issues, this loop can be exited
            break;
        } else {
            CLOG(LOGLEV_RUN, "open failed = ", errno, " = ", strerror(errno));
        }

        // pause before the next attempt
        std::this_thread::sleep_for( std::chrono::milliseconds(200) );
    }

    CLOG(LOGLEV_RUN, "connected to channel ", channel);

//    char data[1024];
//    int size;
//    unsigned int priority;
//    int bytesRead = 0;
//    bytesRead = mq_receive(m_msgQueue, data, 1024, &priority);
//    if(bytesRead <= 0) {
//        CLOG(LOGLEV_RUN, "read confirm message fail (", errno, ") ", strerror(errno));
//        return false;
//    }

 //   t_client = std::thread(&CPOSIXMQClient::threadfunc_client, this);

    return true;
}

bool CPOSIXMQClient::client_disconnect()
{
    if(0 != mq_close(m_mqdes_client))
        return false;
    else
        return true;
}

void CPOSIXMQClient::threadfunc_client()
{
    while(!m_shutdownrequest)
    {
        // listenForData() is a blocking read so we dont need a thread throttle
        if(!listenForData(m_mqdes_client)) {
            CLOG(LOGLEV_RUN, "error on listen");
            break;
        }
    }
}

} // client
} // posix
} // comms

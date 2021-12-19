/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "posix_mq_server.h"

#include "Logger.h"

#include "common.h"

#include <vector>
#include <cstring>
#include <chrono>
#include <thread>

namespace comms {
namespace posix {
namespace server {

//namespace  {
//    const char l_channel_name[] = "/posix_test_mq"; ///\ todo replace with parameter
//}

CPOSIXMQServer::CPOSIXMQServer()
    : m_shutdownrequest(false)
    , m_mqdes_server(-1)
{
    m_sizeOfHeader = sizeof(SMessageHeader);

    if(channel_create())
    {
        t_server = std::thread(&CPOSIXMQServer::threadfunc_server, this);
    }
}

CPOSIXMQServer::~CPOSIXMQServer()
{
    m_shutdownrequest = true;

    mq_unlink(l_channel_name);

    if(t_server.joinable())
        t_server.detach();
}

bool CPOSIXMQServer::channel_create()
{
    // create the posix mq channel
    // will attempt several times

    //disabled mq_attr because using it causes access permission errors
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 33;//m_sizeOfHeader-1; // needs to be smaller than the smallest packet i.e. the header.
    attr.mq_curmsgs = 0;

    //mode_t mode  = S_IRWXU | S_IRWXG | S_IRWXO;
    //int o_flag = O_CREAT | O_RDWR  ;//| O_NONBLOCK ;

#if 1
    m_mqdes_server = mq_open(&l_channel_name[0], O_CREAT|O_RDWR, 0644, &attr);
    //mqd_t m_mqdes_server = mq_open(&l_channel_name[0], O_CREAT|O_RDWR, 0644, &attr);

    if(-1 != m_mqdes_server) {
        CLOG(LOGLEV_RUN, "channel ", l_channel_name, " created");

    } else {
        CLOG(LOGLEV_RUN, "open failed: (", errno, ") ", strerror(errno));
        return false;
    }


#else
    for(int retry_index = 0; retry_index < l_max_num_of_connect_attempts; retry_index++)
    {
        CLOG(LOGLEV_RUN, "channel create attempt = ", retry_index + 1);
        if(l_max_num_of_connect_attempts == retry_index + 1) {
            CLOG(LOGLEV_RUN, "max channel create attempts reached");
            return false;
        }

        //m_mqdes = mq_open(l_channel_name,  o_flag, mode, NULL);
        m_mqdes_server = mq_open(&l_channel_name[0],  o_flag, 0666, NULL);

        if(-1 != m_mqdes_server) {
            // no issues, this loop can be exited
            break;
        } else {
            CLOG(LOGLEV_RUN, "open failed = ", errno, " = ", strerror(errno));
        }

        // pause before the next attempt
        std::this_thread::sleep_for( std::chrono::milliseconds(200) );
    }

    CLOG(LOGLEV_RUN, "channel ", l_channel_name, " created");
#endif

#if 0
    // The channel is open, send the confirmation message
    char confirmMsgBuff;
    SMessageHeader head;
    head.size = 1;
    head.type = EMsgTypCtrl;

    std::vector<char> package;
    package.resize(1 + m_sizeOfHeader);
    std::memcpy(&package[0], &head, m_sizeOfHeader);
    std::memcpy(&package[m_sizeOfHeader], &confirmMsgBuff, 1);
    ssize_t result = transmit(&package[0] , package.size());
    if(result <= 0) {
        CLOG(LOGLEV_RUN, "send fail");
        return false;
    }
#endif
    return true;
}

void CPOSIXMQServer::threadfunc_server()
{
    while(!m_shutdownrequest)
    {
        // listenForData() is a blocking read so we dont need a thread throttle
        if(!listenForData(m_mqdes_server)) {
//                CLOG(LOGLEV_RUN, "error on listen");
            break;
        }
    }
}

} // comms
} // posix
} // client

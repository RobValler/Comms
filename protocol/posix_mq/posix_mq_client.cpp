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

CPOSIXMQClient::CPOSIXMQClient()
{
    m_sizeOfHeader = sizeof(SMessageHeader);

    if(channel_create())
    {
        t_provider_channel_thread = std::thread(&CPOSIXMQClient::threadfunc_client, this);
    }

}

CPOSIXMQClient::~CPOSIXMQClient()
{
    static_cast<void>(client_disconnect());

    mq_unlink(m_provider_channel_name.data());

    m_shutdownrequest = true;

    if(t_provider_channel_thread.joinable())
        t_provider_channel_thread.detach();
}

bool CPOSIXMQClient::channel_create()
{
    // create the posix mq channel
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 33; //m_sizeOfHeader-1; // needs to be smaller than the smallest packet i.e. the header.
    attr.mq_curmsgs = 0;

    //mode_t mode  = S_IRWXU | S_IRWXG | S_IRWXO;
    int o_flag = O_CREAT | O_RDWR  ;//| O_NONBLOCK ;

    m_provider_channel_name = l_mq_channel_name + "_client";
    m_provider_channel_desc = mq_open(m_provider_channel_name.data(), o_flag, 0644, &attr);

    if(-1 != m_provider_channel_desc) {
        CLOG(LOGLEV_RUN, "channel ", m_provider_channel_name, " created");

    } else {
        CLOG(LOGLEV_RUN, "open failed", ERR_STR);
        return false;
    }

    return true;
}

bool CPOSIXMQClient::client_connect(std::string channel)
{
    // ### Connect to remote

    for(int retry_index = 0; retry_index < l_max_num_of_connect_attempts; retry_index++)
    {
        CLOG(LOGLEV_RUN, "channel ", channel, " connect attempt = ", retry_index + 1);
        if(l_max_num_of_connect_attempts == retry_index + 1) {
            CLOG(LOGLEV_RUN, "max channel connect attempts reached");
            return false;
        }

        // open as write only
        m_listener_channel_desc = mq_open(channel.data(), O_WRONLY);

        if(-1 != m_listener_channel_desc) {
            // no issues, this loop can be exited
            break;
        } else {
            CLOG(LOGLEV_RUN, "open failed = ", errno, " = ", strerror(errno));
        }

        // pause before the next attempt
        std::this_thread::sleep_for( std::chrono::milliseconds(200) );
    }

    CLOG(LOGLEV_RUN, "connected to channel ", channel);

    return true;
}

bool CPOSIXMQClient::client_disconnect()
{
    if(0 != mq_close(m_listener_channel_desc))
        return false;
    else
        return true;
}

void CPOSIXMQClient::threadfunc_client()
{
    while(!m_shutdownrequest)
    {
        // listenForData() is a blocking read so we dont need a thread throttle
        if(!listenForData(m_provider_channel_desc)) {
            CLOG(LOGLEV_RUN, "error on listen");
            break;
        }
    }
}

} // client
} // posix
} // comms

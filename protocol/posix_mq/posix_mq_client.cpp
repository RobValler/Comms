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

    if(channel_create("_client"))
    {
        t_provider_channel_thread = std::thread(&CPOSIXMQClient::threadfunc_client, this);
    }
}

CPOSIXMQClient::~CPOSIXMQClient()
{
    static_cast<void>(client_disconnect());

    m_shutdownrequest = true;

    if(t_provider_channel_thread.joinable())
        t_provider_channel_thread.detach();
}

void CPOSIXMQClient::threadfunc_client()
{
    while(!m_shutdownrequest)
    {
        // listenForData() is a blocking read
        if(!listenForData(m_provider_channel_desc))
        {
            CLOG(LOGLEV_RUN, "error on listen");
            break;
        }
    }
}

} // client
} // posix
} // comms

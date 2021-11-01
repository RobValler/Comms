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

#include <vector>
#include <cstring>
#include <chrono>
#include <thread>

namespace comms {
namespace posix {
namespace server {

namespace  {
    const char l_channel_name[] = "/posix_test_mq"; // todo: replace with parameter
    const int l_max_num_of_connect_attempts = 5;
}

// todo: move to common section
enum EMessageType : std::uint8_t
{
    EMsgTypNone = 0,
    EMsgTypCtrl,
    EMsgTypData
};

// todo: move to common section
struct SMessageHeader {
    std::uint32_t size;
    std::uint8_t type;
};

CPOSIXMQServer::CPOSIXMQServer()
    : m_shutdownrequest(false)
    , m_msgQueue(-1)
{
    m_sizeOfHeader = sizeof(SMessageHeader);
    t_server = std::thread(&CPOSIXMQServer::threadfunc_server, this);
//    t_client = std::thread(&CPOSIXMQServer::threadfunc_server, this);

}

CPOSIXMQServer::~CPOSIXMQServer()
{
    m_shutdownrequest = true;

    mq_close(m_msgQueue);

    t_server.join();
//    t_client.join();

}

bool CPOSIXMQServer::channel_create()
{
    // create the posix mq channel
    // will attempt several times

    //disabled mq_attr because using it causes access permission errors
//    mq_attr attrib;
//    attrib.mq_flags = 0;
//    attrib.mq_maxmsg = 16;
//    attrib.mq_msgsize = 1024;
//    attrib.mq_curmsgs = 0;

    mode_t mode  = S_IRWXU | S_IRWXG | S_IRWXO;
    int o_flag = O_CREAT | O_WRONLY  | O_NONBLOCK ;

    for(int retry_index = 0; retry_index < l_max_num_of_connect_attempts; retry_index++)
    {
        CLOG(LOGLEV_RUN, "channel create attempt = ", retry_index + 1);
        if(l_max_num_of_connect_attempts == retry_index + 1) {
            CLOG(LOGLEV_RUN, "max channel create attempts reached");
            return false;
        }

        m_msgQueue = mq_open(l_channel_name,  o_flag, mode, NULL);
        if(-1 != m_msgQueue) {
            // no issues, this loop can be exited
            break;
        } else {
            CLOG(LOGLEV_RUN, "open failed = ", errno, " = ", strerror(errno));
        }

        // pause before the next attempt
        std::this_thread::sleep_for( std::chrono::milliseconds(200) );
    }

    CLOG(LOGLEV_RUN, "channel ", l_channel_name, " created");

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

bool CPOSIXMQServer::recieve(std::vector<char>& data, int& size)
{
    unsigned int priority = 1;

//    int result = mq_receive(m_msgQueue, (char*)data, m_sizeOfHeader, &priority);
//    if(result < 0)
//        return false;
//    else
        return true;
}

bool CPOSIXMQServer::transmit(const char *data, const int size)
{
    (void)size;

    unsigned int priority = 1;
    int result = mq_send(m_msgQueue, (char*)data, 1024, priority);
    if(result < 0)
        return false;
    else
        return true;
}

void CPOSIXMQServer::threadfunc_server()
{
    bool result = false;
    result = channel_create();
    if(result)
        return;
    while(!m_shutdownrequest)
    {
        listenForData();
    }
   // else
     //   CLOG(LOGLEV_RUN, "server_connect failed");
}

bool CPOSIXMQServer::listenForData()
{
    // read the header first
    SMessageHeader header;
    unsigned int priority = 1;

    int result = mq_receive(m_msgQueue, (char*)&header, m_sizeOfHeader, &priority);
    if(result < 0)
        return false;
    else
        return true;
    return true;
}

} // comms
} // posix
} // client

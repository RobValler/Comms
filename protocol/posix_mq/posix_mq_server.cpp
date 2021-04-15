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

namespace  {
    const char l_channel_name[] = "POSIX_TEST_Q";
}


enum EMessageType : std::uint8_t
{
    EMsgTypNone = 0,
    EMsgTypCtrl,
    EMsgTypData
};

struct SMessageHeader {
    std::uint32_t size;
    std::uint8_t type;
};

CPOSIXMQServer::CPOSIXMQServer()
    : m_shutdownrequest(false)
    , msgQueue(-1)
{
    m_sizeOfHeader = sizeof(SMessageHeader);
   // t_server = std::thread(&CPOSIXMQServer::threadfunc_server, this);

    server_connect();
}

CPOSIXMQServer::~CPOSIXMQServer()
{
    m_shutdownrequest = true;
    //t_server.join();

}

bool CPOSIXMQServer::server_connect()
{
    // create the posix mq channel
    // will attempt several times
    const int o_flag = O_RDWR | O_CREAT;
    for(int retry_index = 0; retry_index < 10; retry_index++)
    {
        CLOG(LOGLEV_RUN, "cannel create attempt = ", retry_index + 1);
        if(10 == retry_index + 1) {
            CLOG(LOGLEV_RUN, "max channel create attempts reached");
            return false;
        }

        msgQueue = mq_open(l_channel_name, o_flag);
        if(msgQueue >= 0) {
            break;
        }

        std::this_thread::sleep_for( std::chrono::milliseconds(200) );
    }

    CLOG(LOGLEV_RUN, "channel created");


    // send the confirmation message
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

    return true;
}

bool CPOSIXMQServer::recieve(char** data, int& size)
{
    unsigned int priority = 1;
    int result = mq_receive(msgQueue, (char*)data, size, &priority);
    if(1 == result)
        return false;
    else
        return true;
}


bool CPOSIXMQServer::transmit(const char *data, const int size)
{
    unsigned int priority = 1;
    int result = mq_send(msgQueue, (char*)data, size, priority);
    if(size != result)
        return false;
    else
        return true;
}

void CPOSIXMQServer::threadfunc_server()
{
    bool result = false;
    result = server_connect();
    if(result)
        listenForData();
    else
        CLOG(LOGLEV_RUN, "server_connect failed");
}

bool CPOSIXMQServer::listenForData()
{

    return true;
}

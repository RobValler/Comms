/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

#include <vector>
#include <string>

class IProtocolServer
{
public:
    virtual ~IProtocolServer(){}
    virtual bool client_connect(std::string) =0;
    virtual bool client_disconnect()=0;
    virtual bool recieve(std::vector<char>&, int&) =0;
    virtual bool transmit(const char*, const int) =0;
    virtual int sizeOfReadBuffer() =0;
};

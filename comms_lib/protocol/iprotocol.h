/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

class IProtocol
{
public:
    virtual ~IProtocol(){}
    virtual bool server_connect()=0;
    virtual bool client_connect()=0;
    virtual bool disconnect()=0;
    virtual bool recieve() =0;
    virtual bool transmit() =0;
};

/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

//#include "iprotocol.h"

#include <memory>


namespace google{ namespace protobuf{ class Message; } }
class IProtocolServer;

class CCommServer
{
public:
    CCommServer();
    ~CCommServer();

    bool read(::google::protobuf::Message& message);
    bool write(const ::google::protobuf::Message& message);

private:
    std::shared_ptr<IProtocolServer> m_pProtocolServer;

};

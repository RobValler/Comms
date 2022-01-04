/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "basic.h"
#include "Logger.h"

#include <cstring>

namespace comms {
namespace serial {
namespace basic {

bool CSerialiserBasic::serialise(std::vector<char>& buffer, int& size_of_message, void* incomming_data)
{
    if(0U == size_of_message)
    {
        CLOG(LOGLEV_RUN, "size_of_message was not correctly set");
        return false;
    }
    buffer.resize(size_of_message);
    std::memcpy(&buffer[0], incomming_data, size_of_message);
    return true;
}

bool CSerialiserBasic::deserialise(const std::vector<char>& buffer, int size_of_message, void* outgoing_data)
{
    std::memcpy(outgoing_data, &buffer[0], size_of_message);
    return true;
}

} // basic
} // serial
} // comms

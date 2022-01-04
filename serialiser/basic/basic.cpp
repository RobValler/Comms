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

bool CSerialiserBasic::serialise(void* incomming_data, std::vector<char>& outgoing_data, int& outgoing_size)
{
    if(0U == outgoing_size)
    {
        CLOG(LOGLEV_RUN, "size_of_message was not correctly set");
        return false;
    }
    outgoing_data.resize(outgoing_size);
    std::memcpy(&outgoing_data[0], incomming_data, outgoing_size);
    return true;
}

bool CSerialiserBasic::deserialise(const std::vector<char>& incomming_data, void* outgoing_data, const int outgoing_size)
{
    std::memcpy(outgoing_data, &incomming_data[0], outgoing_size);
    return true;
}

} // basic
} // serial
} // comms

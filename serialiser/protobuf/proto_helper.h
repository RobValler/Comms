/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

#include "iserialiser.h"

namespace comms {
namespace serial {
namespace protobuf {

class CSerialiserProto
        : public ISerialiser
{
public:
    CSerialiserProto();
    ~CSerialiserProto();

    bool serialise(void* incomming_data, std::vector<char>& outgoing_data, int& outgoing_size) override;
    bool deserialise(const std::vector<char>& incomming_data, void* outgoing_data, int&) override;

};

} // protobuf
} // serial
} // comms



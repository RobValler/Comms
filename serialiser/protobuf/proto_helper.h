/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once

#include "iserialiser.h"

namespace comms {
namespace serial {
namespace protobuf {

class CSerialiser
        : public ISerialiser
{
public:
    CSerialiser();
    ~CSerialiser();

    bool serialise(std::vector<char>& buffer, int& size_of_message, void* incomming_data) override;
    bool deserialise(std::vector<char> buffer, int size_of_message, void* outgoing_data) override;

};

} // protobuf
} // serial
} // comms



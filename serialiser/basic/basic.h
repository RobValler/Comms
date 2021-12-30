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
namespace basic {

class CSerialiserBasic
        : public ISerialiser
{
public:
    CSerialiserBasic()=default;                                         // Constructor
    CSerialiserBasic(const CSerialiserBasic&) = delete;                // Copy constructor
    CSerialiserBasic(CSerialiserBasic&&) = delete;                     // Move constructor
    CSerialiserBasic& operator=(const CSerialiserBasic&) = delete;     // Copy assignment operator
    CSerialiserBasic& operator=(CSerialiserBasic&&) = delete;          // Move assignment operator
    virtual ~CSerialiserBasic()=default;                                // Destructor

    bool serialise(std::vector<char>& buffer, int& size_of_message, void* incomming_data) override;
    bool deserialise(const std::vector<char>& buffer, int size_of_message, void* outgoing_data) override;

};

} // basic
} // serial
} // comms



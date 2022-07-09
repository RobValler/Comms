/*****************************************************************
 * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
 *
 * This file is part of the project: Comms
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#pragma once
#include <vector>

class ISerialiser
{
public:
    virtual ~ISerialiser(){}
    virtual bool serialise(void*, std::vector<char>&, int&) =0;
    virtual bool deserialise(const std::vector<char>&, void*, int&) =0;
};

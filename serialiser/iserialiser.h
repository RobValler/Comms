/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
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
    virtual bool serialise(std::vector<char>&, int&, void*) =0;
    virtual bool deserialise(std::vector<char>, int, void*) =0;
};

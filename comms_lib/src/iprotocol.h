/*****************************************************************
 * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

class IProtocol
{
public:
    virtual ~IProtocol(){}
    virtual void read()=0;
    virtual void write()=0;
};

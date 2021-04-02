/*****************************************************************
 * Copyright (C) 2017-2019 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include <gtest/gtest.h>
#include "comms.h"

TEST(Comms, BasicTest)
{
    CComms send, rec;
    send.write();
    send.read();


}

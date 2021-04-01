/*****************************************************************
 * Copyright (C) 2017-2019 Robert Valler - All rights reserved.
 *
 * This file is part of the project: StarterApp
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#include "example.pb.h"

#include <vector>
#include <string>

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    test_msg test;

    std::vector<unsigned char> data;
    std::string str("moose");
    data.assign(str.begin(), str.end());

    google::protobuf::io::ArrayOutputStream array_out( &data[0], data.size() );
    google::protobuf::io::CodedOutputStream coded_out( &array_out );
    coded_out.WriteVarint32( test.ByteSizeLong() );
    test.SerializeToCodedStream( &coded_out );



}

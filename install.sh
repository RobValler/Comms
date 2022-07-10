#!/bin/sh
set -e
rm -rf build || true
mkdir -p build
cd build
conan install .. --build missing
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j16
#test/./comms_test_server &
#test/./comms_test_client 
sudo cmake --install .
cd ..


#!/bin/sh
set -e
mkdir -p build || true
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j16
test/./comms_test_server &
test/./comms_test_client 
killall -9 comms_test_server
killall -9 comms_test_client
sudo cmake --install .
cd ..


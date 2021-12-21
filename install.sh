#!/bin/sh
set -e
mkdir -p build || true
cd build
cmake ..
make -j16
test/./comms_test
sudo cmake --install .
cd ..


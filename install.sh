#!/bin/sh
set -e
rm -rf build || true
mkdir -p build
cd build
mkdir -p release
mkdir -p debug
#build release
cd release
conan install ../.. --build missing
cmake ../.. -DCMAKE_BUILD_TYPE=Release
make -j16
sudo cmake --install .
cd ..
#build debug
cd debug
conan install ../.. --build missing
cmake ../.. -DCMAKE_BUILD_TYPE=Debug
make -j16
cd ../..

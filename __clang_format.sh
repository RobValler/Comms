#!/bin/sh
rm -rf ./build/*
find . -iname *.h -o -iname *.cpp | xargs clang-format -i --style=Chromium

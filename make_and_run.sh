#!/bin/bash
cd `dirname $0`
echo MAKE .....................
make clean
clang-format --style=Chromium -i src/*.c src/*.h
make all
echo RUN ......................
./build/main
echo RC=$?
sleep 2
#!/bin/bash
cd `dirname $0`
echo MAKE .....................
make clean
clang-format --style=Chromium -i src/*.c src/*.h
make
echo RUN ......................
./myraygui_test
echo RC=$?
echo WAIT .....................
read X

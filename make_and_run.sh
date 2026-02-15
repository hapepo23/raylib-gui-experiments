#!/bin/bash
cd `dirname $0`
echo MAKE .....................
make clean
clang-format --style=Chromium -i *.c *.h
make all
echo RUN ......................
./my_raylib_gui
echo RC=$?
sleep 2
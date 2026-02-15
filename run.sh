#!/bin/bash
cd `dirname $0`
echo RUN ......................
./my_raylib_gui
echo RC=$?
echo WAIT .....................
sleep 2

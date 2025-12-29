#!/bin/bash
cd `dirname $0`
echo RUN ......................
./build/main
echo RC=$?
echo WAIT .....................
sleep 2

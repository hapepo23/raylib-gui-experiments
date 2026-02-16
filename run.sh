#!/bin/bash
cd `dirname $0`
echo RUN ......................
./myraygui_test
echo RC=$?
echo WAIT .....................
read X

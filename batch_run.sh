#!/bin/bash
# A simple script
# to run through all pictures in SRC_FOLDER

SRCDIR=./pictures/
DSTDIR=./matrix_example_test/
pwd
for (( n=6; n<=30; n++ ))
do
    ./build/bin/main $SRCDIR$n".bmp" $DSTDIR$n".bmp" --align
done

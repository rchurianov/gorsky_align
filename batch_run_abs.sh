#!/bin/bash
# A simple script
# to run through all pictures in SRC_FOLDER

SRCDIR=/home/roman/projects/vision/gorsky/pictures/
DSTDIR=/home/roman/projects/vision/gorsky/matrix_example_test/

for (( n=6; n<=30; n++ ))
do
    ./build/bin/main $SRCDIR$n".bmp" $DSTDIR$n".bmp" --align
done

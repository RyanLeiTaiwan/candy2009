#!/bin/bash
# Author: ryanlei
# Description: put the "gcc" command in a shell script
#   This script generates multiple execution files for large applications
cd source           
gcc -Wall -o ../hough util.c matrix.c image.c hough.c
gcc -Wall -o ../hog util.c matrix.c image.c hog.c
gcc -Wall -o ../car-train util.c matrix.c image.c car-extract.c car-adaboost.c car-train.c
gcc -Wall -o ../car-detect util.c matrix.c image.c car-extract.c car-detect.c
cd ..

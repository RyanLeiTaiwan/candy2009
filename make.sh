#!/bin/bash
# Author: ryanlei
# Description: put the "gcc" command in a shell script
#   This script generates multiple execution files for large applications
           
# gcc -Wall -o hough source/util.c source/matrix.c source/image.c source/hough.c
# gcc -Wall -o hog source/util.c source/matrix.c source/image.c source/hog.c
gcc -Wall -o car-train source/util.c source/matrix.c source/image.c source/car-extract.c source/car-train.c

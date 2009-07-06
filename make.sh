#!/bin/bash
# Author: ryanlei
# Description: put the "gcc" command in a shell script
#   This script generates multiple execution files for large applications
           
gcc -Wall -o hough util.c matrix.c image.c hough.c
gcc -Wall -o hog util.c matrix.c image.c hog.c

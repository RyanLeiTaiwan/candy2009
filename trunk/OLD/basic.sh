#!/bin/bash
# Author: ryanlei
# Description: put the "gcc" command in a shell script
#   This script generate "basic" components for testing:
#     util.*, matrix.*, and image.*
           
gcc -Wall -o basic source/util.c source/matrix.c source/image.c

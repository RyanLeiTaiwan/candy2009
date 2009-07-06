#!/bin/bash
# Author: ryanlei
# Description: put the "gcc" command in a shell script
#   This script generate "basic" for testing:
#     util.*, matrix.*, and image.*
           
gcc -Wall -o basic util.c matrix.c image.c

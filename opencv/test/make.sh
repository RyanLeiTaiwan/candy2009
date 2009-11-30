#!/bin/bash
# Author: ryanlei
# Description: put the "gcc" command in a shell script
#   This script generates multiple executables for the project.

# IMPORTANT: This script only work on Mac OS X 10.6+ on an Intel 64/32-bit machine.
# The OpenCV 2.0 library should be installed via Macports using the method provided by
# http://blog.mgrundmann.com/?p=5
# The header files and libraries are at their Macports default locations.
# The program ONLY WORKS ON Mac for now because of the "say" system call.
g++ -Wall -o hello hello.cpp -I/opt/local/include -L/opt/local/lib -lcxcore -lhighgui

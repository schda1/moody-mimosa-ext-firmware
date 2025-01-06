#!/bin/bash

PRJ=$(pwd)
CPPUTEST=$PRJ/tests/cpputest

# Initialize git submodules (ST Drivers, cpputest)
git submodule init
git submodule update --init --recursive

# Slight modification of the cpputest configure script
source scripts/add_ranlib_to_cpputest_configure.sh

# Build cpputest
cd $CPPUTEST
mkdir cpputest_build
cd cpputest_build
autoreconf .. -i
../configure
make

# Copy built libraries
mkdir -p $CPPUTEST/lib
cp -f $CPPUTEST/cpputest_build/lib/libCppUTest.a $CPPUTEST/lib/libCppUTest.a
cp -f $CPPUTEST/cpputest_build/lib/libCppUTestExt.a $CPPUTEST/lib/libCppUTestExt.a

# Return to the project root folder
cd $PRJ
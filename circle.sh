#!/usr/bin/env bash

set -e

# compile
mkdir build
cd build
cmake -DLIBSOCKET_CODE_COVERAGE=ON ..
make -j4

# test
test/libsocket_test

# code coverage
bash <(curl -s https://codecov.io/bash) -t 647d3dc9-cc6e-4d24-8cc4-66764a63527c
#!/usr/bin/env bash

set -e

# compile
mkdir test/build
cd test/build
cmake -DLIBSOCKET_TEST_CODE_COVERAGE=ON ..
make

# test
./test_libsocket

# code coverage
bash <(curl -s https://codecov.io/bash) -t 647d3dc9-cc6e-4d24-8cc4-66764a63527c
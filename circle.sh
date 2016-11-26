#!/usr/bin/env bash

set -e

# compile
mkdir build
cd build
cmake -DLIBCHEN_CODE_COVERAGE=ON ..
make -j4

# test
test/libchen_test --data=../test/data

# code coverage
bash <(curl -s https://codecov.io/bash)
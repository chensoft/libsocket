#!/usr/bin/env bash

set -e

# compile
mkdir test/build
cd test/build
cmake -DLIBCHEN_TEST_CODE_COVERAGE=ON ..
make

# test
./test_libchen --data=../data

# code coverage
bash <(curl -s https://codecov.io/bash)
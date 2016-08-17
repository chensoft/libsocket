#!/usr/bin/env bash

set -e

mkdir test/build
cd test/build
cmake ..
make
./test_libsocket
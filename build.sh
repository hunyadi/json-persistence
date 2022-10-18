#!/bin/bash
set -e

mkdir -p build
pushd build > /dev/null
cmake -D CMAKE_BUILD_TYPE=Release ..
make

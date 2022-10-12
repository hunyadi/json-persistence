#!/bin/bash
set -e

mkdir -p build
pushd build > /dev/null
cmake ..
make

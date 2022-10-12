@echo off
setlocal
if not exist build mkdir build
pushd build
cmake ..

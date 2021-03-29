#!/bin/sh
cd `git rev-parse --show-toplevel`

clang-format -i --style=file src/*.cpp src/*.h include/cfd/*.h src/capi/*.cpp src/capi/*.h include/cfdc/*.h

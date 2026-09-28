#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
mkdir -p build/host logs
c++ -std=c++11 -O1 -g -fno-omit-frame-pointer -fpack-struct=1 \
 -fsanitize=address,undefined -fno-sanitize=alignment -DGC_DEBUG -DGC_TEST -Isrc/engine \
 src/engine/*.cpp tests/engine_tests.cpp -o build/host/engine_tests
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ./build/host/engine_tests

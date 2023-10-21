#!/bin/bash

export CXX=clang++
export CC=clang
set -e
trap "echo >&2 failed" ERR
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug 
make -Cbuild -j12 TrustWalletCore
make -Cbuild -j12 tests

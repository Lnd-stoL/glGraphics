#!/bin/bash

cd build-release-bin
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j 4


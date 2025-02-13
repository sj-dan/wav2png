#!/bin/bash

cmake -S . -B dist -DCMAKE_BUILD_TYPE=Release
cmake --build dist -j8

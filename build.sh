#!/bin/bash

CURRENT_FOLDER=$PWD

BUILD_FOLDER='./dist'

rm -rf "$BUILD_FOLDER"
mkdir -p "$BUILD_FOLDER"

cd "$BUILD_FOLDER"
cmake "$CURRENT_FOLDER"
make -j$(nproc --all)

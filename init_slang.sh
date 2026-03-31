#!/usr/bin/env bash
set -e

echo "Updating submodules..."
git submodule update --init --recursive

echo "Entering Slang directory..."
cd Engine/Libs/Slang

echo "Fetching Slang tags..."
git fetch https://github.com/shader-slang/slang.git 'refs/tags/*:refs/tags/*'

echo "Configuring Slang..."
cmake --preset default -DSLANG_USE_SCCACHE=ON -DSLANG_ENABLE_TESTS=OFF -DSLANG_ENABLE_EXAMPLES=OFF -DSLANG_ENABLE_RHI=OFF

echo "Building Slang..."
cmake --build --preset release

echo "Installing Slang..."
cd build
cmake --build . --target install

echo "Done."
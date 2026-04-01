#!/usr/bin/env bash
set -e

echo "Updating submodules..."
git submodule update --init --recursive

echo "Entering Slang directory..."
cd Engine/Libs/Slang

echo "Configuring Slang..."
cmake --preset default

echo "Building Slang..."
cmake --build --preset release

echo "Installing Slang..."
cd build
cmake --build . --target install

echo "Done."
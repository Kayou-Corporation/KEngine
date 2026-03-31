#!/usr/bin/env bash
set -e

echo "Updating submodules..."
git submodule update --init --recursive

echo "Done."
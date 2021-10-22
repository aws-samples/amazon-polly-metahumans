#!/bin/bash

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
SDK_REPO_DIR="$SCRIPT_DIR/aws-sdk-cpp"
SDK_BUILD_DIR="$SDK_REPO_DIR/_build"
SDK_INSTALL_DIR="$SDK_REPO_DIR/_install"
MODULE_MAC_DIR="$SCRIPT_DIR/Mac"

# Clone the repo
cd "$SCRIPT_DIR"
git clone --recurse-submodules https://github.com/aws/aws-sdk-cpp.git

# Create build and install directories where we will build and install the SDK to
cd "$SDK_REPO_DIR"
mkdir "$SDK_BUILD_DIR"
mkdir "$SDK_INSTALL_DIR"

# Build the SDK
cd "$SDK_BUILD_DIR"
cmake "$SDK_REPO_DIR" -DCMAKE_INSTALL_PREFIX="$SDK_INSTALL_DIR" -DBUILD_ONLY="polly" -DCUSTOM_MEMORY_MANAGEMENT=ON
make

# Install the SDK
make install

# Remove any previous builds from the platform specific directory
mkdir $MODULE_MAC_DIR
rm -rf "$MODULE_MAC_DIR"/*

# Copy the new build to the platform specific directory
cp -R "$SDK_INSTALL_DIR"/include "$MODULE_MAC_DIR"/include
mkdir "$MODULE_MAC_DIR"/lib
ls -1 "$SDK_INSTALL_DIR"/lib/*.dylib | xargs -L1 -I{} cp {} "$MODULE_MAC_DIR"/lib/

# Remove the cloned repo, build, and install directory.
rm -rf "$SDK_REPO_DIR"

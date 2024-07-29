#!/bin/bash

# Set the installation prefix and target
BASE_DIR=$(pwd)
TOOLCHAIN_DIR="$BASE_DIR/i686"
SRC_DIR="$TOOLCHAIN_DIR/src"
BUILD_DIR="$TOOLCHAIN_DIR/build"
PREFIX="$TOOLCHAIN_DIR"
TARGET=i686-elf
PATH="$PREFIX/bin:$PATH"

# Versions of the tools
GCC_VERSION=14.1.0
BINUTILS_VERSION=2.42

# Directories for sources and build
SRC_DIR="$HOME/src"
BUILD_DIR="$SRC_DIR/build"
mkdir -p $PREFIX $SRC_DIR $BUILD_DIR

# Function to download and extract a tarball
download_and_extract() {
    local url=$1
    local tarball=$(basename $url)
    local dirname=${tarball%.tar.*}

    if [ ! -f $SRC_DIR/$tarball ]; then
        wget -P $SRC_DIR $url
    fi
    if [ ! -d $SRC_DIR/$dirname ]; then
        tar -xf $SRC_DIR/$tarball -C $SRC_DIR
    fi
}

# Download GCC and Binutils sources
download_and_extract https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz
download_and_extract https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz

# Build and install Binutils
mkdir -p $BUILD_DIR/binutils
cd $BUILD_DIR/binutils
$SRC_DIR/binutils-$BINUTILS_VERSION/configure --target=$TARGET --prefix=$PREFIX --with-sysroot --disable-nls --disable-werror
make
make install

# Build and install GCC (Bootstrap)
mkdir -p $BUILD_DIR/gcc
cd $BUILD_DIR/gcc
$SRC_DIR/gcc-$GCC_VERSION/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make install-gcc
make all-target-libgcc
make install-target-libgcc

echo "Cross-compiler for $TARGET installed in $PREFIX"

# Verify the installation
$PREFIX/bin/$TARGET-gcc --version

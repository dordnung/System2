#!/bin/bash
set -eu

SYSTEM2_DIR="$(pwd)"
BUILD_DIR="$SYSTEM2_DIR/build-linux"


if [[ ! -d "$BUILD_DIR" ]]; then
	mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR" || exit

# OpenSSL
echo "Building openssl"
if [[ ! -f "openssl-1.1.1g.tar.gz" ]]; then
	wget https://www.openssl.org/source/openssl-1.1.1g.tar.gz && tar -xzf openssl-1.1.1g.tar.gz
fi

cd openssl-1.1.1g
setarch i386 ./config -m32 no-shared no-tests && make
mkdir lib && cp ./*.a lib/
cd "$BUILD_DIR" || exit

# Zlib
echo "Building zlib"
if [[ ! -f "zlib1211.zip" ]]; then
	wget http://zlib.net/zlib1211.zip && unzip -q zlib1211.zip
fi
	
cd zlib-1.2.11
CFLAGS=-m32 ./configure -static && make
mkdir include && mkdir lib && cp ./*.h include/ && cp libz.a lib
cd "$BUILD_DIR" || exit

# Libidn
echo "Building libidn"
if [[ ! -f "libidn2-2.2.0.tar.gz" ]]; then
	wget https://ftp.gnu.org/gnu/libidn/libidn2-2.2.0.tar.gz && tar -xzf libidn2-2.2.0.tar.gz
fi

cd libidn2-2.2.0
CFLAGS=-m32 -std=gnu11 ./configure --disable-shared --enable-static --disable-doc && make
mkdir include && cp lib/*.h include/ && cp lib/.libs/libidn2.a lib
cd "$BUILD_DIR" || exit

# LibCurl
echo "Building libcurl"
if [[ ! -f "curl-7.70.0.zip" ]]; then
	wget https://curl.haxx.se/download/curl-7.70.0.zip && unzip -q curl-7.70.0.zip
fi

cd curl-7.70.0
./configure --with-ssl="$BUILD_DIR/openssl-1.1.1g" --with-zlib="$BUILD_DIR/zlib-1.2.11" \
 --with-libidn2="$BUILD_DIR/libidn2-2.2.0" --disable-shared --enable-static --disable-rtsp \
 --disable-ldap --disable-ldaps --disable-manual --disable-libcurl-option --without-librtmp \
 --without-libssh2 --without-nghttp2 --without-gssapi --host=i386-pc-linux-gnu CFLAGS=-m32 && make all ca-bundle
cd "$BUILD_DIR" || exit

# SourceMod
echo "Getting sourcemod"
if [[ ! -d "sourcemod-${SMBRANCH}" ]]; then
	git clone https://github.com/alliedmodders/sourcemod --recursive --branch "$SMBRANCH" --single-branch "sourcemod-${SMBRANCH}"
fi


echo "Building system2"
cd "$SYSTEM2_DIR" || exit
make SMSDK="$BUILD_DIR/sourcemod-${SMBRANCH}" OPENSSL="$BUILD_DIR/openssl-1.1.1g" ZLIB="$BUILD_DIR/zlib-1.2.11" IDN="$BUILD_DIR/libidn2-2.2.0" CURL="$BUILD_DIR/curl-7.70.0"
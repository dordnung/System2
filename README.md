System2
=======

[![Build Status](https://api.travis-ci.com/dordnung/System2.svg)](https://travis-ci.com/dordnung/System2)

System2 extends Sourcemod with a easy HTTP/FTP Request API and also provides some useful system commands, which missing in the Sourcemod API.

Binaries and more information can be found on [alliedmods.net](https://forums.alliedmods.net/showthread.php?t=146019)

## How-to build: ##

### On Linux: ###
- **Set build path**
  1. `export BUILD_DIR=$HOME`
  2. `cd $BUILD_DIR`

- **Build openssl**
  1. `wget https://www.openssl.org/source/openssl-1.1.1k.tar.gz && tar -xvzf openssl-1.1.1k.tar.gz`
  2. `cd openssl-1.1.1k`
  3. `setarch i386 ./config -m32 no-shared && make`
  4. `mkdir lib && cp *.a lib/`
  5. `cd $BUILD_DIR`

- **Build zlib**
  1. `wget http://zlib.net/zlib1211.zip && unzip zlib1211.zip`
  2. `cd zlib-1.2.11`
  3. `CFLAGS=-m32 ./configure -static && make`
  4. `mkdir include && mkdir lib && cp *.h include/ && cp libz.a lib`
  5. `cd $BUILD_DIR`

- **Build libidn**
  1. `wget https://ftp.gnu.org/gnu/libidn/libidn2-2.2.0.tar.gz && tar -xvzf libidn2-2.2.0.tar.gz`
  2. `cd libidn2-2.2.0`
  3. `CFLAGS=-m32 ./configure --disable-shared --enable-static --disable-doc && make`
  4. `mkdir include && cp lib/*.h include/ && cp lib/.libs/libidn2.a lib`
  5. `cd $BUILD_DIR`

- **Build libcurl**
  1. `wget https://curl.se/download/curl-7.76.0.zip && unzip curl-7.76.0.zip`
  2. `cd curl-7.76.0`
  3. `./configure --with-ssl=$BUILD_DIR/openssl-1.1.1k --with-zlib=$BUILD_DIR/zlib-1.2.11 --with-libidn2=$BUILD_DIR/libidn2-2.2.0 --disable-shared --enable-static --disable-rtsp --disable-ldap --disable-ldaps --disable-manual --disable-libcurl-option --without-librtmp --without-libssh2 --without-nghttp2 --without-gssapi --host=i386-pc-linux-gnu CFLAGS=-m32 && make all ca-bundle`
  4. **DO NOT INSTALL IT!**
  5. `cd $BUILD_DIR`

- **Get Sourcemod 1.10**
  1. `git clone https://github.com/alliedmodders/sourcemod --recursive --branch 1.10-dev --single-branch sourcemod-1.10`

- **Build system2**
  1. `git clone https://github.com/dordnung/System2`
  2. `cd System2`
  3. `make SMSDK=$BUILD_DIR/sourcemod-1.10 OPENSSL=$BUILD_DIR/openssl-1.1.1k ZLIB=$BUILD_DIR/zlib-1.2.11 IDN=$BUILD_DIR/libidn2-2.2.0 CURL=$BUILD_DIR/curl-7.76.0`

### On Windows (Visual Studio 2019): ###
- **Build zlib**
  1. Download zlib from `https://zlib.net/zlib1211.zip` and unzip to some folder
  2. Open the `Developer Command Prompt for VS 2019` at the `zlib-1.2.11` folder
  3. Type `vcvarsall.bat x86` and press ENTER
  4. Type `nmake /f win32/Makefile.msc LOC=-MT` and press ENTER
  5. Type `md lib include` and press ENTER
  6. Type `copy /Y zlib.lib lib` and press ENTER
  7. Type `copy /Y *h include` and press ENTER
  8. Add a new system variable named `ZLIB` pointing to the `zlib-1.2.11` folder

- **Build libcurl**
  1. Download curl from `https://curl.se/download/curl-7.76.0.zip` and unzip to some folder
  2. Reopen the `Developer Command Prompt for VS 2019` at the `curl-7.76.0` folder
  3. Type `vcvarsall.bat x86` and press ENTER
  4. Type `cd winbuild` and press ENTER
  5. Type `nmake /f Makefile.vc mode=static WITH_ZLIB=static ZLIB_PATH=%ZLIB% RTLIBCFG=static VC=16 MACHINE=x86` and press ENTER
  6. Add a new system variable named `CURL` pointing to the `curl-7.76.0/builds/libcurl-vc16-x86-release-static-zlib-static-ipv6-sspi-schannel` folder

- **Get Sourcemod 1.10**
  1. Retrieve Sourcemod 1.10 with: `git clone https://github.com/alliedmodders/sourcemod --recursive --branch 1.10-dev --single-branch sourcemod-1.10`
  2. Add a new system variable named `SOURCEMOD` with the path to the sourcemod-1.10 folder

- **Build System2**
  1. Retrieve System2 with: `git clone https://github.com/dordnung/System2`
  2. Reopen the `Developer Command Prompt for VS 2019` at the `system2` folder
  3. Type `vcvarsall.bat x86` and press ENTER
  4. Type `msbuild msvc19/system2.sln /p:Platform="win32"` and press ENTER
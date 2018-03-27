System2
=======

System2 extends Sourcemod with a easy HTTP/FTP Request API and also provides some useful system commands, which missing in the Sourcemod API.

Binaries and more information can be found on [alliedmods.net](https://forums.alliedmods.net/showthread.php?t=146019)

## How-to build: ##

### On Linux: ###
- **Set path to build**
  1. `export BUILD_DIR=$HOME`
  2. `cd $BUILD_DIR`

- **Build openssl**
  1. `wget https://www.openssl.org/source/openssl-1.1.0e.tar.gz && tar -xvzf openssl-1.1.0e.tar.gz`
  2. `cd openssl-1.1.0e`
  3. `setarch i386 ./config -m32 no-shared && make`
  4. `cd $BUILD_DIR`

- **Build zlib**
  1. `wget http://zlib.net/zlib1211.zip && unzip zlib1211.zip`
  2. `cd zlib-1.2.11`
  3. `CFLAGS=-m32 ./configure -static && make`
  4. `cd $BUILD_DIR`

- **Build libcurl**
  1. `wget https://curl.haxx.se/download/curl-7.57.0.zip && unzip curl-7.57.0.zip`
  2. `cd curl-7.57.0`
  3. `env LIBS="-ldl" CPPFLAGS="-I$BUILD_DIR/zlib-1.2.11" LDFLAGS="-L$BUILD_DIR/openssl-1.1.0e -L$BUILD_DIR/zlib-1.2.11" ./configure --with-ssl=$BUILD_DIR/openssl-1.1.0e --with-zlib=$BUILD_DIR/zlib-1.2.11 --disable-shared --enable-static --disable-rtsp --disable-ldap --disable-ldaps --disable-sspi --disable-tls-srp --disable-manual --disable-proxy --disable-libcurl-option --without-librtmp --without-libidn --without-libssh2 --without-nghttp2 --without-gssapi --host=i386-pc-linux-gnu CFLAGS=-m32 CC=/usr/bin/gcc && make`
  4. **DO NOT INSTALL IT!**
  4. `cd $BUILD_DIR`

- **Get Sourcemod and Sourcepawn**
  1. `wget https://github.com/alliedmodders/sourcemod/archive/master.zip -O sourcemod.zip && unzip sourcemod.zip`
  2. `wget https://github.com/alliedmodders/sourcepawn/archive/master.zip -O sourcepawn.zip && unzip sourcepawn.zip`
  3. `cp -r sourcepawn-master/* sourcemod-master/sourcepawn/`

- **Build system2**
  1. `wget https://github.com/dordnung/System2/archive/master.zip -O system2.zip && unzip system2.zip`
  2. `cd System2-master`
  3. `make SMSDK=$BUILD_DIR/sourcemod-master OPENSSL=$BUILD_DIR/openssl-1.1.0e ZLIB=$BUILD_DIR/zlib-1.2.11 CURL=$BUILD_DIR/curl-7.57.0`

### On Windows (Visual Studio 2015/2017): ###
- **Build zlib**
  1. Download zlib from `https://zlib.net/zlib1211.zip` and unzip to some folder
  2. Open the Visual Studio Command Prompt (x86!) at `zlib-1.2.11`
  3. Type `nmake /f win32/Makefile.msc LOC=-MT` and press ENTER
  4. Type `md ..\deps\lib ..\deps\include` and press ENTER
  5. Type `copy /Y zlib.lib ..\deps\lib\zlib_a.lib` and press ENTER
  6. Type `copy /Y *h ..\deps\include` and press ENTER

- **Build libcurl**
  1. Download curl from `https://curl.haxx.se/download/curl-7.57.0.zip` and unzip to same folder
  2. Type `cd ../curl-7.57.0/winbuild` in the Visual Studio Command Prompt and press ENTER
  3. Type `nmake /f Makefile.vc mode=static WITH_ZLIB=static RTLIBCFG=static VC=14 MACHINE=x86` and press ENTER
  4. Add a new system variable named `CURL` pointing to the `curl-7.57.0/builds/libcurl-vc14-x86-release-static-zlib-static-ipv6-sspi-winssl` folder

- **Get Sourcemod and Sourcepawn**
  1. Download sourcemod from `https://github.com/alliedmodders/sourcemod/archive/master.zip` and unzip
  2. Download sourcepawn from `https://github.com/alliedmodders/sourcepawn/archive/master.zip`, unzip it and move  the content of the folder to the extracted sourcemod/sourcepawn folder
  3. Add a new system variable named `SOURCEMOD` with the path to sourcemod

- **Build system2**
  1. Download System2 from `https://github.com/dordnung/System2/archive/master.zip` and unzip
  2. Open `msvc17/system2.sln` 
  3. Build the project.
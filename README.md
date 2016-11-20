System2
=======

System2 extends Sourcemod with usefully internet and server natives.

Binaries and information can be found on alliedmods: [https://forums.alliedmods.net/showthread.php?t=146019](https://forums.alliedmods.net/showthread.php?t=146019)

## How-to build: ##

### On Linux: ###
- **Set path to build**
  1. `export BUILD_DIR=$HOME`
  2. `cd $BUILD_DIR`

- **Build openssl**
  1. `wget https://www.openssl.org/source/openssl-1.1.0c.tar.gz && tar -xvzf openssl-1.1.0c.tar.gz`
  2. `cd openssl-1.1.0c`
  3. `setarch i386 ./config -m32 no-shared && make`
  4. `cd $BUILD_DIR`

- **Build zlib**
  1. `wget http://zlib.net/zlib128.zip && unzip zlib128.zip`
  2. `cd zlib-1.2.8`
  3. `CFLAGS=-m32 ./configure -static && make`
  4. `cd $BUILD_DIR`

- **Build libcurl**
  1. `wget http://curl.haxx.se/download/curl-7.51.0.zip && unzip curl-7.51.0.zip`
  2. `cd curl-7.51.0`
  3. `env LIBS="-ldl" CPPFLAGS="-I$BUILD_DIR/zlib-1.2.8" LDFLAGS="-L$BUILD_DIR/openssl-1.1.0c -L$BUILD_DIR/zlib-1.2.8" ./configure --with-ssl=$BUILD_DIR/openssl-1.1.0c --with-zlib=$BUILD_DIR/zlib-1.2.8 --disable-shared --enable-static --disable-rtsp --disable-ldap --disable-ldaps --disable-sspi --disable-tls-srp --without-librtmp --without-libidn --without-libssh2 --without-nghttp2 --without-gssapi --host=i686-pc-linux-gnu CFLAGS=-m32 CC=/usr/bin/gcc && make`
  4. `cd $BUILD_DIR`

- **Get Sourcemod and Sourcepawn**
  - `wget https://github.com/alliedmodders/sourcemod/archive/master.zip -O sourcemod.zip && unzip sourcemod.zip`
  - `wget https://github.com/alliedmodders/sourcepawn/archive/master.zip -O sourcepawn.zip && unzip sourcepawn.zip`

- **Build system2**
  1. `wget https://github.com/popoklopsi/System2/archive/master.zip -O system2.zip && unzip system2.zip`
  2. `cd System2-master`
  3. `make SMSDK=$BUILD_DIR/sourcemod-master OPENSSL=$BUILD_DIR/openssl-1.1.0c ZLIB=$BUILD_DIR/zlib-1.2.8 CURL=$BUILD_DIR/curl-7.51.0`

### On Windows (Visual Studio 2015): ###
- **Build libcurl**
  1. Download curl from `http://curl.haxx.se/download/curl-7.51.0.zip` and unzip
  2. Add VS to the system PATH:
    - For example: `C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin` 
  3. Open command line at `curl-7.51.0/winbuild`
  4. Type `vcvars32.bat` and press ENTER
  5. Type `nmake /f Makefile.vc mode=static VC=14 MACHINE=x86` and press ENTER
  6. Add a new system variable named `CURL` pointing to the 'curl-7.51.0/builds/libcurl-vc14-x86-release-static-ipv6-sspi-winssl' folder

- **Get Sourcemod and Sourcepawn **
  1. Download sourcemod from `https://github.com/alliedmodders/sourcemod/archive/master.zip` and unzip
  1. Download sourcepawn from `https://github.com/alliedmodders/sourcepawn/archive/master.zip`, unzip it and move it to the extracted sourcemod folder
  2. Add a new system variable named `SOURCEMOD` with the path to sourcemod

- **Build system2**
  1. Download System2 from `https://github.com/popoklopsi/System2/archive/master.zip` and unzip
  2. Open `msvc13/system2.sln` 
  3. Build the project.
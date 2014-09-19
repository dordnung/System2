System2
=======

System2 extends Sourcemod with usefull internet and server functions.

### How-to build on apt based systems: ###

1. `sudo apt-get install libssl-dev`
2. `wget http://curl.haxx.se/download/curl-7.38.0.tar.gz`
3. `tar -xvzf curl-7.38.0.tar.gz`
4. `cd curl-7.38.0`
5. `./configure --disable-shared --enable-static --disable-rtsp --disable-ldap --disable-ldaps --disable-sspi --disable-tls-srp --without-librtmp --without-libidn --without-libssh2 --without-nghttp2 --without-gssapi`
6. `make`
7. `sudo make install`
8. Goto Makefile and: `make all`


Binaries and information can be found on alliedmods: [https://forums.alliedmods.net/showthread.php?t=146019](https://forums.alliedmods.net/showthread.php?t=146019)

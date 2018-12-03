set SYSTEM2_DIR=%cd%
set BUILD_DIR=%SYSTEM2_DIR%\build-windows
set VCVARSALL="C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"


if not exist "%BUILD_DIR%" (
	mkdir "%BUILD_DIR%"
)


cd %BUILD_DIR%


if "%VSCMD_VER%"=="" (
	set MAKE=
	set CC=
	set CXX=
	call %VCVARSALL% x86 8.1
)



REM Zlib
echo "Building zlib"
if not exist "zlib.zip" (
	curl https://zlib.net/zlib1211.zip -o zlib.zip
	unzip -q zlib.zip
)

cd zlib-1.2.11
nmake /f win32/Makefile.msc LOC=-MT
md lib include
copy /Y zlib.lib lib
copy /Y *h include
set ZLIB=%cd%

cd %BUILD_DIR%




REM Libcurl
echo "Building curl"
if not exist "curl.zip" (
	curl https://curl.haxx.se/download/curl-7.62.0.zip -o curl.zip
	unzip -q curl.zip
)

cd curl-7.62.0\winbuild
nmake /f Makefile.vc mode=static WITH_ZLIB=static ZLIB_PATH=%ZLIB% RTLIBCFG=static VC=15 MACHINE=x86

cd ..\builds\libcurl-vc15-x86-release-static-zlib-static-ipv6-sspi-winssl
set CURL=%cd%

cd %BUILD_DIR%




REM Sourcemod
echo "Getting sourcemod"
if not exist "sourcemod-%SMBRANCH%" (
	git clone https://github.com/alliedmodders/sourcemod --recursive --branch %SMBRANCH% --single-branch sourcemod-%SMBRANCH%
)

cd sourcemod-%SMBRANCH%
set SOURCEMOD19=%cd%

cd %BUILD_DIR%





REM System2
echo "Building system2"

cd %SYSTEM2_DIR%
msbuild msvc17/system2.sln /p:Platform="win32"



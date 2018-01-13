# (C)2004-2010 SourceMod Development Team
# Makefile written by David "BAILOPAN" Anderson

###########################################
### EDIT THESE PATHS FOR YOUR OWN SETUP ###
###########################################

SMSDK = ../sourcemod-central
OPENSSL = ../openssl
ZLIB = ../zlib
CURL = ../curl

#####################################
### EDIT BELOW FOR OTHER PROJECTS ###
#####################################

PROJECT = system2

#Uncomment for Metamod: Source enabled extension
#USEMETA = true

OBJECTS = 3rdparty/crc/crc.cpp 3rdparty/md5/md5.cpp
OBJECTS += handler/ExecuteCallbackHandler.cpp Handler.cpp handler/RequestHandler.cpp handler/ResponseCallbackHandler.cpp
OBJECTS += legacy/LegacyNatives.cpp
OBJECTS += legacy/threads/LegacyCommandThread.cpp legacy/threads/LegacyDownloadThread.cpp legacy/threads/LegacyFTPThread.cpp legacy/threads/LegacyPageThread.cpp
OBJECTS += legacy/threads/callbacks/LegacyCommandCallback.cpp legacy/threads/callbacks/LegacyDownloadCallback.cpp
OBJECTS += natives/CommonNatives.cpp natives/ExecuteNatives.cpp natives/FTPRequest.cpp natives/HTTPRequest.cpp natives/Request.cpp natives/RequestNatives.cpp natives/ResponseNatives.cpp
OBJECTS += sdk/smsdk_ext.cpp
OBJECTS += threads/CopyThread.cpp threads/ExecuteThread.cpp threads/FTPRequestThread.cpp threads/HTTPRequestThread.cpp threads/RequestThread.cpp
OBJECTS += threads/callbacks/CopyCallback.cpp threads/callbacks/ExecuteCallback.cpp threads/callbacks/FTPResponseCallback.cpp threads/callbacks/HTTPResponseCallback.cpp threads/callbacks/ProgressCallback.cpp threads/callbacks/ResponseCallback.cpp
OBJECTS += extension.cpp

##############################################
### CONFIGURE ANY OTHER FLAGS/OPTIONS HERE ###
##############################################

C_OPT_FLAGS = -DNDEBUG -O3 -funroll-loops -pipe -fno-strict-aliasing
C_DEBUG_FLAGS = -D_DEBUG -DDEBUG -g -ggdb3
C_GCC4_FLAGS = -fvisibility=hidden
CPP_GCC4_FLAGS = -fvisibility-inlines-hidden
CPP = gcc
CPP_OSX = clang

##########################
### SDK CONFIGURATIONS ###
##########################

INCLUDE += -I. -I.. -Isdk -Ithreads -Ihash -I$(SMSDK)/public -I$(SMSDK)/sourcepawn/include -I$(SMSDK)/core -I$(CURL)/include
LINK += -m32 -lm -ldl -lrt -lstdc++ $(CURL)/lib/.libs/libcurl.a $(OPENSSL)/libssl.a $(OPENSSL)/libcrypto.a $(ZLIB)/libz.a

CFLAGS += -std=c++0x -DPOSIX -DCURL_STATICLIB -Dstricmp=strcasecmp -D_stricmp=strcasecmp -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp \
	-D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp -DCOMPILER_GCC -Wall -Werror \
	-Wno-overloaded-virtual -Wno-switch -Wno-unused -Wno-parentheses -msse -DSOURCEMOD_BUILD -DHAVE_STDINT_H -m32
CPPFLAGS += -Wno-non-virtual-dtor -fno-exceptions -fno-rtti

################################################
### DO NOT EDIT BELOW HERE FOR MOST PROJECTS ###
################################################

BINARY = $(PROJECT).ext.so

ifeq "$(DEBUG)" "true"
	BIN_DIR = Debug
	CFLAGS += $(C_DEBUG_FLAGS)
else
	BIN_DIR = Release
	CFLAGS += $(C_OPT_FLAGS)
endif

OS := $(shell uname -s)

ifeq "$(OS)" "Darwin"
	CPP = $(CPP_OSX)
	CFLAGS += -DOSX -D_OSX
	LINK += -dynamiclib -lstdc++ -mmacosx-version-min=10.5
else
	CFLAGS += -D_LINUX
	LINK += -shared
endif

IS_CLANG := $(shell $(CPP) --version | head -1 | grep clang > /dev/null && echo "1" || echo "0")

ifeq "$(IS_CLANG)" "1"
	CPP_MAJOR := $(shell $(CPP) --version | grep clang | sed "s/.*version \([0-9]\)*\.[0-9]*.*/\1/")
	CPP_MINOR := $(shell $(CPP) --version | grep clang | sed "s/.*version [0-9]*\.\([0-9]\)*.*/\1/")
else
	CPP_MAJOR := $(shell $(CPP) -dumpversion >&1 | cut -b1)
	CPP_MINOR := $(shell $(CPP) -dumpversion >&1 | cut -b3)
endif

# If not clang
ifeq "$(IS_CLANG)" "0"
	CFLAGS += -mfpmath=sse
endif

# Clang || GCC >= 4
ifeq "$(shell expr $(IS_CLANG) \| $(CPP_MAJOR) \>= 4)" "1"
	CFLAGS += $(C_GCC4_FLAGS)
	CPPFLAGS += $(CPP_GCC4_FLAGS)
endif

# Clang >= 3 || GCC >= 4.7
ifeq "$(shell expr $(IS_CLANG) \& $(CPP_MAJOR) \>= 3 \| $(CPP_MAJOR) \>= 4 \& $(CPP_MINOR) \>= 7)" "1"
	CFLAGS += -Wno-delete-non-virtual-dtor
endif

# OS is Linux and not using clang
ifeq "$(shell expr $(OS) \= Linux \& $(IS_CLANG) \= 0)" "1"
	LINK += -static-libgcc
endif

OBJ_BIN := $(OBJECTS:%.cpp=$(BIN_DIR)/%.o)

# This will break if we include other Makefiles, but is fine for now. It allows
#  us to make a copy of this file that uses altered paths (ie. Makefile.mine)
#  or other changes without mucking up the original.
MAKEFILE_NAME := $(CURDIR)/$(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))

$(BIN_DIR)/%.o: %.cpp
	$(CPP) $(INCLUDE) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<

all: check
	mkdir -p $(BIN_DIR)/3rdparty/crc
	mkdir -p $(BIN_DIR)/3rdparty/md5
	mkdir -p $(BIN_DIR)/handler
	mkdir -p $(BIN_DIR)/legacy
	mkdir -p $(BIN_DIR)/legacy/threads
	mkdir -p $(BIN_DIR)/legacy/threads/callbacks
	mkdir -p $(BIN_DIR)/natives
	mkdir -p $(BIN_DIR)/sdk
	mkdir -p $(BIN_DIR)/threads
	mkdir -p $(BIN_DIR)/threads/callbacks
	$(MAKE) -f $(MAKEFILE_NAME) extension

check:

extension: check $(OBJ_BIN)
	$(CPP) $(INCLUDE) $(OBJ_BIN) $(LINK) -o $(BIN_DIR)/$(BINARY)

debug:
	$(MAKE) -f $(MAKEFILE_NAME) all DEBUG=true

default: all

clean: check
	rm -rf $(BIN_DIR)/*.o
	rm -rf $(BIN_DIR)/3rdparty/crc/*.o
	rm -rf $(BIN_DIR)/3rdparty/md5/*.o
	rm -rf $(BIN_DIR)/handler/*.o
	rm -rf $(BIN_DIR)/legacy/*.o
	rm -rf $(BIN_DIR)/legacy/threads/*.o
	rm -rf $(BIN_DIR)/legacy/threads/callbacks/*.o
	rm -rf $(BIN_DIR)/natives/*.o
	rm -rf $(BIN_DIR)/sdk/*.o
	rm -rf $(BIN_DIR)/threads/*.o
	rm -rf $(BIN_DIR)/threads/callbacks/*.o
	rm -rf $(BIN_DIR)/$(BINARY)


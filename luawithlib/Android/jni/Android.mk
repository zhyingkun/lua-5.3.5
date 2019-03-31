LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LUA_PROJECT_PATH := ../../..

LOCAL_MODULE := luawithlib
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(LUA_PROJECT_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(LUA_PROJECT_PATH)/lualib/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(LUA_PROJECT_PATH)/cmod/luasocket/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(LUA_PROJECT_PATH)/cmod/luanet/include
LOCAL_CFLAGS :=  -g -std=gnu99 -DLUA_USE_LINUX

# lualib
LUALIB_PATH := $(LUA_PROJECT_PATH)/lualib
LUALIB_CORE_SRC := ${LUALIB_PATH}/core
LUALIB_LIBRARIES_SRC := ${LUALIB_PATH}/libraries
LOCAL_SRC_FILES := $(LUALIB_CORE_SRC)/lopcodes.c \
					$(LUALIB_CORE_SRC)/lstate.c \
					$(LUALIB_CORE_SRC)/lobject.c \
					$(LUALIB_CORE_SRC)/lvm.c \
					$(LUALIB_CORE_SRC)/lfunc.c \
					$(LUALIB_CORE_SRC)/lstring.c \
					$(LUALIB_CORE_SRC)/lundump.c \
					$(LUALIB_CORE_SRC)/lctype.c \
					$(LUALIB_CORE_SRC)/ltable.c \
					$(LUALIB_CORE_SRC)/ldump.c \
					$(LUALIB_CORE_SRC)/lgc.c \
					$(LUALIB_CORE_SRC)/lzio.c \
					$(LUALIB_CORE_SRC)/lmem.c \
					$(LUALIB_CORE_SRC)/lcode.c \
					$(LUALIB_CORE_SRC)/lapi.c \
					$(LUALIB_CORE_SRC)/ldebug.c \
					$(LUALIB_CORE_SRC)/lparser.c \
					$(LUALIB_CORE_SRC)/llex.c \
					$(LUALIB_CORE_SRC)/ltm.c \
					$(LUALIB_CORE_SRC)/ldo.c \
					$(LUALIB_LIBRARIES_SRC)/lauxlib.c \
					$(LUALIB_LIBRARIES_SRC)/lbaselib.c \
					$(LUALIB_LIBRARIES_SRC)/lbitlib.c \
					$(LUALIB_LIBRARIES_SRC)/lcorolib.c \
					$(LUALIB_LIBRARIES_SRC)/ldblib.c \
					$(LUALIB_LIBRARIES_SRC)/linit.c \
					$(LUALIB_LIBRARIES_SRC)/liolib.c \
					$(LUALIB_LIBRARIES_SRC)/lmathlib.c \
					$(LUALIB_LIBRARIES_SRC)/loadlib.c \
					$(LUALIB_LIBRARIES_SRC)/loslib.c \
					$(LUALIB_LIBRARIES_SRC)/lstrlib.c \
					$(LUALIB_LIBRARIES_SRC)/ltablib.c \
					$(LUALIB_LIBRARIES_SRC)/lutf8lib.c

# lua cmod
# luasocket
LUASOCKET_PATH := $(LUA_PROJECT_PATH)/cmod/luasocket/src
LOCAL_SRC_FILES += $(LUASOCKET_PATH)/buffer.c \
					$(LUASOCKET_PATH)/compat.c \
					$(LUASOCKET_PATH)/io.c \
					$(LUASOCKET_PATH)/timeout.c \
					$(LUASOCKET_PATH)/select.c \
					$(LUASOCKET_PATH)/udp.c \
					$(LUASOCKET_PATH)/mime.c \
					$(LUASOCKET_PATH)/tcp.c \
					$(LUASOCKET_PATH)/auxiliar.c \
					$(LUASOCKET_PATH)/inet.c \
					$(LUASOCKET_PATH)/luasocket.c \
					$(LUASOCKET_PATH)/except.c \
					$(LUASOCKET_PATH)/options.c \
					$(LUASOCKET_PATH)/serial.c \
					$(LUASOCKET_PATH)/usocket.c \
					$(LUASOCKET_PATH)/unixdgram.c \
					$(LUASOCKET_PATH)/unixstream.c \
					$(LUASOCKET_PATH)/unix.c

# luanet
LUASOCKET_PATH := $(LUA_PROJECT_PATH)/cmod/luanet/src
LOCAL_SRC_FILES += $(LUASOCKET_PATH)/luanet.c

include $(BUILD_SHARED_LIBRARY)

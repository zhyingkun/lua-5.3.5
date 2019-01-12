LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LUA_PROJECT_PATH := ../../..

LOCAL_MODULE := luawithlib
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(LUA_PROJECT_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(LUA_PROJECT_PATH)/cmod/luasocket/include
LOCAL_CFLAGS :=  -g -std=gnu99 -DLUA_USE_LINUX

# lualib
LUALIB_PATH := $(LUA_PROJECT_PATH)/lualib/src
LOCAL_SRC_FILES := $(LUALIB_PATH)/lauxlib.c \
					$(LUALIB_PATH)/liolib.c \
					$(LUALIB_PATH)/lopcodes.c \
					$(LUALIB_PATH)/lstate.c \
					$(LUALIB_PATH)/lobject.c \
					$(LUALIB_PATH)/lmathlib.c \
					$(LUALIB_PATH)/loadlib.c \
					$(LUALIB_PATH)/lvm.c \
					$(LUALIB_PATH)/lfunc.c \
					$(LUALIB_PATH)/lstrlib.c \
					$(LUALIB_PATH)/linit.c \
					$(LUALIB_PATH)/lstring.c \
					$(LUALIB_PATH)/lundump.c \
					$(LUALIB_PATH)/lctype.c \
					$(LUALIB_PATH)/ltable.c \
					$(LUALIB_PATH)/ldump.c \
					$(LUALIB_PATH)/loslib.c \
					$(LUALIB_PATH)/lgc.c \
					$(LUALIB_PATH)/lzio.c \
					$(LUALIB_PATH)/ldblib.c \
					$(LUALIB_PATH)/lutf8lib.c \
					$(LUALIB_PATH)/lmem.c \
					$(LUALIB_PATH)/lcorolib.c \
					$(LUALIB_PATH)/lcode.c \
					$(LUALIB_PATH)/ltablib.c \
					$(LUALIB_PATH)/lbitlib.c \
					$(LUALIB_PATH)/lapi.c \
					$(LUALIB_PATH)/lbaselib.c \
					$(LUALIB_PATH)/ldebug.c \
					$(LUALIB_PATH)/lparser.c \
					$(LUALIB_PATH)/llex.c \
					$(LUALIB_PATH)/ltm.c \
					$(LUALIB_PATH)/ldo.c

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

include $(BUILD_SHARED_LIBRARY)

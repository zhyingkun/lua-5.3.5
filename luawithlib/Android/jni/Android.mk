LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LUA_PROJECT_PATH := $(LOCAL_PATH)/../../..

LOCAL_MODULE := luawithlib
LOCAL_MODULE_FILENAME := libluawithlib

LOCAL_C_INCLUDES := $(LUA_PROJECT_PATH)/liblua/include
LOCAL_C_INCLUDES += $(LUA_PROJECT_PATH)/liblua/core
LOCAL_C_INCLUDES += $(LUA_PROJECT_PATH)/cmod/luasocket/mime
LOCAL_C_INCLUDES += $(LUA_PROJECT_PATH)/cmod/luasocket/socket
LOCAL_C_INCLUDES += $(LUA_PROJECT_PATH)/cmod/luasocket/socket/unix
LOCAL_C_INCLUDES += $(LUA_PROJECT_PATH)/cmod/luasocket/unix

LOCAL_CFLAGS := -std=gnu99 -DLUA_USE_LINUX

LOCAL_SRC_FILES := $(shell bash $(LOCAL_PATH)/ListSRC.sh ${LUA_PROJECT_PATH})

# LOCAL_CPPFLAGS := -std=c++11 # for cpp
# TARGET_FORMAT_STRING_CFLAGS := -Wformat # -Werror=format-security

# compiled inside flag: -Wl,--whole-archive -Wl,--no-whole-archive
# LOCAL_WHOLE_STATIC_LIBRARIES := libxxx # use with other library
# outside whole archive flag
# LOCAL_STATIC_LIBRARIES :=
# LOCAL_SHARED_LIBRARIES :=

include $(BUILD_SHARED_LIBRARY)

# search for lib Android.mk in environment NDK_MODULE_PATH (path separate with ':')
# $(call import-module,libxxx) 

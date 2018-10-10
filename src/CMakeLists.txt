# Note: This CMakeLists.txt is for mac only
cmake_minimum_required(VERSION 3.0)
project(lua-5.3.5)
add_compile_options(-std=gnu99 -O2 -Wall -Wextra -DLUA_COMPAT_5_2 -DLUA_USE_MACOSX)
# set(CMAKE_CXX_FLAGS xxx)
# set(CMAKE_C_FLAGS xxx)
# add_subdirectory(src)
set(LUA_LIB_SRC lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c lutf8lib.c loadlib.c linit.c)
add_library(lua_lib STATIC ${LUA_LIB_SRC})
set_target_properties(lua_lib PROPERTIES OUTPUT_NAME "lua")

add_executable(lua lua.c ${LUA_LIB_SRC})
target_link_libraries(lua m readline)

add_executable(luac luac.c ${LUA_LIB_SRC})
target_link_libraries(luac m readline)

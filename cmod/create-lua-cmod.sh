#! /usr/bin/env bash

# Template for Write features to file
# writeFilePath="FileName"
# echo "Write features to $writeFilePath ..."
# cat << EOF > $writeFilePath
# This is the file context
# EOF
# exitWhileError "Write features to $writeFilePath failed"

usage(){
    echo "
    [sudo] sh $0 <LuaCModName>

        <LuaCModName>: The c module name you want to create.
        "
}
exitWhileError(){
    if test $? -ne 0 ; then
        echo "
        Error Message: $1
        Please Check The Error And Test Again! Exit Now.
        "
        exit
    fi
}

if test $# -ne 1 ; then
    usage
    exit
fi

cmodName=$1

if test -d $cmodName ; then
	echo \"$cmodName\"" module already exist!!! Exit Now."
	exit
fi

echo "Create Directories ..."
mkdir -p $cmodName $cmodName/src
exitWhileError "Create Directories failed"

echo "Create files ..."
touch $cmodName/CMakeLists.txt $cmodName/src/$cmodName.c
exitWhileError "Create files failed"

# Note: all '$' will be convert, so all '$' in CMakeLists.txt muse escape
writeFilePath=$cmodName/CMakeLists.txt
echo "Write features to $writeFilePath ..."
cat << EOF > $writeFilePath
cmake_minimum_required(VERSION 3.0)
project($cmodName
	VERSION 0.1.0
	DESCRIPTION "Lua $cmodName module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)
if("\${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "CMAKE_BUILD_TYPE is \${CMAKE_BUILD_TYPE}")

if(APPLE)
	set(CMAKE_C_FLAGS_DEBUG   "-std=gnu99 -Wall -Wextra -DLUA_USE_MACOSX -g")
	set(CMAKE_C_FLAGS_RELEASE "-std=gnu99 -Wall -Wextra -DLUA_USE_MACOSX -O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS_DEBUG   "-std=gnu99 -Wall -Wextra -DLUA_USE_LINUX -g")
	set(CMAKE_C_FLAGS_RELEASE "-std=gnu99 -Wall -Wextra -DLUA_USE_LINUX -O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS_DEBUG   "-std=gnu99 -Wall -DLUA_BUILD_AS_DLL -g")
	set(CMAKE_C_FLAGS_RELEASE "-std=gnu99 -Wall -DLUA_BUILD_AS_DLL -O2 -DNDEBUG")
endif()

include_directories(../../include)
aux_source_directory(./src HELLOMOD_SRC)
source_group(src FILES \${HELLOMOD_SRC})

add_library(\${PROJECT_NAME} MODULE \${HELLOMOD_SRC})
target_link_libraries(\${PROJECT_NAME} lualib)
EOF
exitWhileError "Write features to $writeFilePath failed"

writeFilePath=$cmodName/src/$cmodName.c
echo "Write features to $writeFilePath ..."
cat << EOF > $writeFilePath
/* Lua C Library */

#define hello_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>

// typedef int (*lua_CFunction) (lua_State *L);
static int printHello(lua_State *L)
{
	(void)L;
	printf("Hello, here are in $cmodName mod\n");
	return 0;
}

static const luaL_Reg luaLoadFun[] = {
	{"printHello", printHello},
	{NULL, NULL}
};

LUAMOD_API int luaopen_libhello(lua_State *L)
{
	luaL_newlib(L, luaLoadFun);
	return 1;
}
EOF
exitWhileError "Write features to $writeFilePath failed"

writeFilePath=CMakeLists.txt
echo "Append features to $writeFilePath ..."
cat << EOF >> $writeFilePath
add_subdirectory($cmodName)
set_property(TARGET $cmodName PROPERTY FOLDER "cmod")
EOF
exitWhileError "Append features to $writeFilePath failed"

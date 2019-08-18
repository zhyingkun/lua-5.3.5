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
cmodNameUpperCase=`echo ${cmodName} | tr '[a-z]' '[A-Z]'`

if test -d ${cmodName} ; then
	echo \"${cmodName}\"" module already exist!!! Exit Now."
	exit
fi

echo "Create Directories ..."
mkdir -p ${cmodName} ${cmodName}/src
exitWhileError "Create Directories failed"

echo "Create files ..."
touch ${cmodName}/CMakeLists.txt ${cmodName}/src/${cmodName}.c
exitWhileError "Create files failed"

# Note: all '$' will be convert, so all '$' in CMakeLists.txt muse escape
writeFilePath=${cmodName}/CMakeLists.txt
echo "Write features to $writeFilePath ..."
cat << EOF > $writeFilePath
cmake_minimum_required(VERSION 3.0)
project(${cmodName}
	VERSION 0.1.0
	# DESCRIPTION "Lua ${cmodName} module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)
if("\${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "CMakeLists.txt for \${PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE is \${CMAKE_BUILD_TYPE}")

# LUA_BUILD_AS_DLL is only available for windows lualib and cmod library, not for user
if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -DLUA_BUILD_AS_DLL")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories(../../include)
include_directories(../../lualib/include)
aux_source_directory(./src ${cmodNameUpperCase}MOD_SRC)
source_group(src FILES \${${cmodNameUpperCase}MOD_SRC})

add_library(\${PROJECT_NAME} MODULE \${${cmodNameUpperCase}MOD_SRC})
set_target_properties(\${PROJECT_NAME} PROPERTIES
	FOLDER "cmod"
	# OUTPUT_NAME \${PROJECT_NAME}
	# VERSION "0.1.0"
	# SOVERSION "0.1.0"
	INSTALL_RPATH \${CMAKE_INSTALL_PREFIX}/lib
	POSITION_INDEPENDENT_CODE ON)

target_link_libraries(\${PROJECT_NAME} lualib)

install(TARGETS \${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
EOF
exitWhileError "Write features to $writeFilePath failed"

writeFilePath=${cmodName}/src/${cmodName}.c
echo "Write features to $writeFilePath ..."
cat << EOF > $writeFilePath
/* Lua C Library */

#define ${cmodName}_c
#define LUA_LIB // for export function

#include <lprefix.h> // must include first

#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>

// typedef int (*lua_CFunction) (lua_State *L);
static int printHello(lua_State *L)
{
	(void)L;
	printf("Hello, here are in ${cmodName} mod\n");
	return 0;
}

static const luaL_Reg luaLoadFun[] = {
	{"printHello", printHello},
	{NULL, NULL}
};

LUAMOD_API int luaopen_lib${cmodName}(lua_State *L)
{
	luaL_newlib(L, luaLoadFun);
	return 1;
}
EOF
exitWhileError "Write features to $writeFilePath failed"

writeFilePath=CMakeLists.txt
echo "Append features to $writeFilePath ..."
cat << EOF >> $writeFilePath
add_subdirectory(${cmodName})
EOF
exitWhileError "Append features to $writeFilePath failed"

echo "All done."

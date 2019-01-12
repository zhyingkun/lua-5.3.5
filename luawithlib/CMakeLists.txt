cmake_minimum_required(VERSION 3.0)
project(luawithlib
	VERSION 1.0.0
	DESCRIPTION "Lua with library"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -DLUA_USE_MACOSX")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -DLUA_USE_LINUX")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -DLUA_BUILD_AS_DLL")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories(../include)

aux_source_directory(../lualib/src LUALIB_SRC)
set(LUA_SOCKET_PATH "../cmod/luasocket/src")
set(LUA_SOCKET_SRC
	${LUA_SOCKET_PATH}/auxiliar.c
	${LUA_SOCKET_PATH}/except.c
	${LUA_SOCKET_PATH}/luasocket.c
	${LUA_SOCKET_PATH}/select.c
	${LUA_SOCKET_PATH}/timeout.c
	${LUA_SOCKET_PATH}/buffer.c
	${LUA_SOCKET_PATH}/inet.c
	${LUA_SOCKET_PATH}/mime.c
	${LUA_SOCKET_PATH}/udp.c
	${LUA_SOCKET_PATH}/compat.c
	${LUA_SOCKET_PATH}/io.c
	${LUA_SOCKET_PATH}/options.c
	${LUA_SOCKET_PATH}/tcp.c)
if(WIN32)
	set(PLATFORM_SOCKET_SRC ${LUA_SOCKET_PATH}/wsocket.c)
else(WIN32)
	set(PLATFORM_SOCKET_SRC
		${LUA_SOCKET_PATH}/serial.c
		${LUA_SOCKET_PATH}/usocket.c
		${LUA_SOCKET_PATH}/unixdgram.c
		${LUA_SOCKET_PATH}/unixstream.c
		${LUA_SOCKET_PATH}/unix.c)
endif(WIN32)
set(LUASOCKET_SRC ${LUA_SOCKET_SRC} ${PLATFORM_SOCKET_SRC})

source_group(lualib FILES ${LUALIB_SRC})
source_group(luasocket FILES ${LUASOCKET_SRC})

#dynamic load library  .so .bundle
add_library(${PROJECT_NAME} MODULE ${LUALIB_SRC} ${LUASOCKET_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
	# OUTPUT_NAME "luawithlib"
	# VERSION "1.0.0"
	# SOVERSION "1.0.0"
	POSITION_INDEPENDENT_CODE ON)

if(APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES BUNDLE TRUE)
endif(APPLE)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(${PROJECT_NAME} m dl)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")
if(WIN32)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif(WIN32)

cmake_minimum_required(VERSION 3.0)
project(luasocket
	VERSION 3.0
	# DESCRIPTION "Lua socket c module"
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

include_directories(../../include)
include_directories(./include)

set(LUA_SOCKET_PATH "./src")
set(LUA_SOCKET_INCLUDE_PATH "./include")
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
set(LUA_SOCKET_HEADER
	${LUA_SOCKET_INCLUDE_PATH}/auxiliar.h
	${LUA_SOCKET_INCLUDE_PATH}/except.h
	${LUA_SOCKET_INCLUDE_PATH}/luasocket.h
	${LUA_SOCKET_INCLUDE_PATH}/select.h
	${LUA_SOCKET_INCLUDE_PATH}/timeout.h
	${LUA_SOCKET_INCLUDE_PATH}/buffer.h
	${LUA_SOCKET_INCLUDE_PATH}/inet.h
	${LUA_SOCKET_INCLUDE_PATH}/mime.h
	${LUA_SOCKET_INCLUDE_PATH}/udp.h
	${LUA_SOCKET_INCLUDE_PATH}/compat.h
	${LUA_SOCKET_INCLUDE_PATH}/io.h
	${LUA_SOCKET_INCLUDE_PATH}/options.h
	${LUA_SOCKET_INCLUDE_PATH}/tcp.h)
if(WIN32)
    set(PLATFORM_SOCKET_SRC ${LUA_SOCKET_PATH}/wsocket.c)
    set(PLATFORM_SOCKET_HEADER ${LUA_SOCKET_INCLUDE_PATH}/wsocket.h)
else(WIN32)
	set(PLATFORM_SOCKET_SRC
		${LUA_SOCKET_PATH}/serial.c
		${LUA_SOCKET_PATH}/usocket.c
		${LUA_SOCKET_PATH}/unixdgram.c
		${LUA_SOCKET_PATH}/unixstream.c
		${LUA_SOCKET_PATH}/unix.c)
	set(PLATFORM_SOCKET_HEADER
		${LUA_SOCKET_INCLUDE_PATH}/usocket.h
		${LUA_SOCKET_INCLUDE_PATH}/unixdgram.h
		${LUA_SOCKET_INCLUDE_PATH}/unixstream.h
		${LUA_SOCKET_INCLUDE_PATH}/unix.h)
endif(WIN32)

set(LUASOCKETMOD_SRC ${LUA_SOCKET_SRC} ${PLATFORM_SOCKET_SRC})
set(LUASOCKETMOD_HEADER ${LUA_SOCKET_HEADER} ${PLATFORM_SOCKET_HEADER})

source_group(src FILES ${LUASOCKETMOD_SRC})
source_group(include FILES ${LUASOCKETMOD_HEADER})

add_library(${PROJECT_NAME} MODULE ${LUASOCKETMOD_SRC} ${LUASOCKETMOD_HEADER})
set_target_properties(${PROJECT_NAME} PROPERTIES
	# OUTPUT_NAME "luasocket"
	# VERSION "3.0"
	# SOVERSION "3.0"
	POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${PROJECT_NAME} lualib)

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif(WIN32)

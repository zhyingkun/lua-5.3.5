cmake_minimum_required(VERSION 3.0)
project(luasocket
	VERSION 3.0
	DESCRIPTION "Lua socket c module"
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
set(LUA_SOCKET_SRC
	./src/auxiliar.c
	./src/except.c
	./src/luasocket.c
	./src/select.c
	./src/timeout.c
	./src/buffer.c
	./src/inet.c
	./src/mime.c
	./src/udp.c
	./src/compat.c
	./src/io.c
	./src/options.c
	./src/tcp.c)
set(LUA_SOCKET_HEADER
	./src/auxiliar.h
	./src/except.h
	./src/luasocket.h
	./src/select.h
	./src/timeout.h
	./src/buffer.h
	./src/inet.h
	./src/mime.h
	./src/udp.h
	./src/compat.h
	./src/io.h
	./src/options.h
	./src/tcp.h)
if(WIN32)
    set(PLATFORM_SOCKET_SRC ./src/wsocket.c)
    set(PLATFORM_SOCKET_HEADER ./src/wsocket.h)
else(WIN32)
	set(PLATFORM_SOCKET_SRC
		./src/serial.c
		./src/usocket.c
		./src/unixdgram.c
		./src/unixstream.c
		./src/unix.c)
	set(PLATFORM_SOCKET_HEADER
		./src/usocket.h
		./src/unixdgram.h
		./src/unixstream.h
		./src/unix.h)
endif(WIN32)
set(LUASOCKETMOD_SRC ${LUA_SOCKET_SRC} ${PLATFORM_SOCKET_SRC})
set(LUASOCKETMOD_HEADER ${LUA_SOCKET_HEADER} ${PLATFORM_SOCKET_HEADER})
source_group(src FILES ${LUASOCKETMOD_SRC})
source_group(header FILES ${LUASOCKETMOD_HEADER})
add_library(${PROJECT_NAME} MODULE ${LUASOCKETMOD_SRC} ${LUASOCKETMOD_HEADER})
set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "luasocket"
	VERSION "3.0"
	SOVERSION "3.0"
	POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${PROJECT_NAME} lualib)

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		PREFIX "lib")
endif(WIN32)

cmake_minimum_required(VERSION 3.0)
project(luawithlib
	VERSION 1.0.0
	# DESCRIPTION "Lua with library"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
# message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# LUA_USE_MACOSX and LUA_USE_LINUX are only available for lua.c and liblua, not for user
# LUA_BUILD_AS_DLL are for all windows components, include liblua, cmod library, and user exe which use liblua
if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -DLUA_USE_MACOSX")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -DLUA_USE_LINUX")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "/Wall /DLUA_BUILD_AS_DLL")
	set(CMAKE_C_FLAGS_DEBUG   "/ZI /Od")
	set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

include_directories(../liblua/include)
include_directories(../liblua/core)

aux_source_directory(../liblua/core LUA_CORE_SRC)
aux_source_directory(../liblua/libraries LUA_LIBRARIES_SRC)
set(LIBLUA_SRC ${LUA_CORE_SRC} ${LUA_LIBRARIES_SRC})

include_directories(../cmod/luasocket/mime)
include_directories(../cmod/luasocket/socket)
aux_source_directory(../cmod/luasocket/mime LUASOCKET_MIME_SRC)
aux_source_directory(../cmod/luasocket/socket LUASOCKET_SOCKET_SRC)

if(WIN32)
	include_directories(../cmod/luasocket/socket/win)
	aux_source_directory(../cmod/luasocket/socket/win LUASOCKET_SOCKET_WIN_SRC)
	set(LUASOCKET_PLATFORM ${LUASOCKET_SOCKET_WIN_SRC})
else(WIN32)
	include_directories(../cmod/luasocket/unix)
	include_directories(../cmod/luasocket/socket/unix)
	aux_source_directory(../cmod/luasocket/socket/unix LUASOCKET_SOCKET_UNIX_SRC)
	aux_source_directory(../cmod/luasocket/serial LUASOCKET_SERIAL_SRC)
	aux_source_directory(../cmod/luasocket/unix LUASOCKET_UNIX_SRC)
	set(LUASOCKET_PLATFORM
		${LUASOCKET_SOCKET_UNIX_SRC}
		${LUASOCKET_SERIAL_SRC}
		${LUASOCKET_UNIX_SRC}
	)
endif(WIN32)

set(LUASOCKET_SRC ${LUASOCKET_MIME_SRC} ${LUASOCKET_SOCKET_SRC} ${LUASOCKET_PLATFORM})

source_group(liblua FILES ${LIBLUA_SRC})
source_group(luasocket FILES ${LUASOCKET_SRC})

#dynamic load library  .so .bundle : MODULE
add_library(${PROJECT_NAME} SHARED ${LIBLUA_SRC} ${LUASOCKET_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
	# OUTPUT_NAME "luawithlib"
	# VERSION "1.0.0"
	# SOVERSION "1.0.0"
	POSITION_INDEPENDENT_CODE ON
)

if(APPLE)
	target_compile_definitions(${PROJECT_NAME} PRIVATE UNIX_HAS_SUN_LEN)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(${PROJECT_NAME} m dl)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif()

# target_compile_definitions(${PROJECT_NAME} PRIVATE LUASOCKET_DEBUG)

# install(TARGETS ${PROJECT_NAME}
# 	RUNTIME DESTINATION bin
# 	LIBRARY DESTINATION lib
# 	ARCHIVE DESTINATION lib
# )

cmake_minimum_required(VERSION 3.0)
project(luasocket
	VERSION 3.0
	# DESCRIPTION "Lua socket c module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
# message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# LUA_BUILD_AS_DLL are for all windows components, include liblua, cmod library, and user exe which use liblua
if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "-Wall -DLUA_BUILD_AS_DLL")
	set(CMAKE_C_FLAGS_DEBUG   "/ZI /Od")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories(../../liblua/include)

include_directories(./mime)
include_directories(./socket)
aux_source_directory(./mime LUASOCKET_MIME_SRC)
aux_source_directory(./socket LUASOCKET_SOCKET_SRC)
file(GLOB LUASOCKET_MIME_HEADERS "./mime/*.h")
file(GLOB LUASOCKET_SOCKET_HEADERS "./socket/*.h")
source_group(mime FILES ${LUASOCKET_MIME_SRC} ${LUASOCKET_MIME_HEADERS})
source_group(socket FILES ${LUASOCKET_SOCKET_SRC} ${LUASOCKET_SOCKET_HEADERS})

if(WIN32)
	include_directories(./socket/win)
	aux_source_directory(./socket/win LUASOCKET_SOCKET_WIN_SRC)
	file(GLOB LUASOCKET_SOCKET_WIN_HEADERS "./socket/win/*.h")
	source_group(socket FILES ${LUASOCKET_SOCKET_WIN_SRC} ${LUASOCKET_SOCKET_WIN_HEADERS})
	set(LUASOCKET_PLATFORM ${LUASOCKET_SOCKET_WIN_SRC} ${LUASOCKET_SOCKET_WIN_HEADERS})
else(WIN32)
	include_directories(./unix)
	include_directories(./socket/unix)
	aux_source_directory(./socket/unix LUASOCKET_SOCKET_UNIX_SRC)
	aux_source_directory(./serial LUASOCKET_SERIAL_SRC)
	aux_source_directory(./unix LUASOCKET_UNIX_SRC)
	file(GLOB LUASOCKET_SOCKET_UNIX_HEADERS "./socket/unix/*.h")
	file(GLOB LUASOCKET_UNIX_HEADERS "./unix/*.h")
	source_group(socket FILES ${LUASOCKET_SOCKET_UNIX_SRC} ${LUASOCKET_SOCKET_UNIX_HEADERS})
	source_group(serial FILES ${LUASOCKET_SERIAL_SRC})
	source_group(unix FILES ${LUASOCKET_UNIX_SRC} ${LUASOCKET_UNIX_HEADERS})
	set(LUASOCKET_PLATFORM
		${LUASOCKET_SOCKET_UNIX_SRC}
		${LUASOCKET_SOCKET_UNIX_HEADERS}
		${LUASOCKET_SERIAL_SRC}
		${LUASOCKET_UNIX_SRC}
		${LUASOCKET_UNIX_HEADERS}
	)
endif(WIN32)

add_library(${PROJECT_NAME} MODULE
	${LUASOCKET_MIME_SRC}
	${LUASOCKET_SOCKET_SRC}
	${LUASOCKET_MIME_HEADERS}
	${LUASOCKET_SOCKET_HEADERS}
	${LUASOCKET_PLATFORM}
)
set_target_properties(${PROJECT_NAME} PROPERTIES
	FOLDER "cmod"
	# OUTPUT_NAME "luasocket"
	# VERSION "3.0"
	# SOVERSION "3.0"
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
	POSITION_INDEPENDENT_CODE ON
)
target_link_libraries(${PROJECT_NAME} liblua)

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
endif(WIN32)

if(APPLE)
	target_compile_definitions(${PROJECT_NAME} PRIVATE UNIX_HAS_SUN_LEN)
endif(APPLE)

# target_compile_definitions(${PROJECT_NAME} PRIVATE LUASOCKET_DEBUG)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION cmods/5.3
	ARCHIVE DESTINATION cmods/5.3
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION runner/cmods)

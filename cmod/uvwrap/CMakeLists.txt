cmake_minimum_required(VERSION 3.0)
project(uvwrap
	VERSION 0.1.0
	# DESCRIPTION "Lua uvwrap module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# LUA_BUILD_AS_DLL are for all windows components, include liblua, cmod library, and user exe which use liblua
if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -fvisibility=hidden -Wstrict-prototypes -Wno-unused-parameter -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_DARWIN_UNLIMITED_SELECT=1 -D_DARWIN_USE_64_BIT_INODE=1")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -fvisibility=hidden -Wstrict-prototypes -Wno-unused-parameter -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE -D_POSIX_C_SOURCE=200112")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "-Wall -DLUA_BUILD_AS_DLL /W4 -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x0600")
	# set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories(../../liblua/include)
include_directories(../../liblua/core)
include_directories(./wrap)
include_directories(./include)
include_directories(./src)

aux_source_directory(./wrap UVWRAP_MOD_SRC)
file(GLOB UVWRAP_HEADERS "./wrap/*.h")
source_group(wrap FILES ${UVWRAP_MOD_SRC} ${UVWRAP_HEADERS})


file(GLOB UV_PUBLIC_HEADERS "./include/*.h")
source_group(include FILES ${UV_PUBLIC_HEADERS})
file(GLOB UV_PUBLIC_UV_HEADERS "./include/uv/*.h")
source_group(include\\uv FILES ${UV_PUBLIC_UV_HEADERS})
include_directories(./src)
file(GLOB UV_PRIVATE_HEADERS "./src/*.h")
source_group(src FILES ${UV_PRIVATE_HEADERS})
if(APPLE)
	include_directories(./src/unix)
	file(GLOB UV_HEADERS_UNIX "./src/unix/*.h")
	source_group(unix FILES ${UV_HEADERS_UNIX})
	# include_directories(./src/unix/macosx)
	# file(GLOB UV_HEADERS_UNIX_MACOSX "./src/unix/macosx/*.h")
	# source_group(macosx FILES ${UV_HEADERS_UNIX_MACOSX})
	list(APPEND UV_PRIVATE_HEADERS ${UV_HEADERS_UNIX})
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	include_directories(./src/unix)
	file(GLOB UV_HEADERS_UNIX "./src/unix/*.h")
	source_group(unix FILES ${UV_HEADERS_UNIX})
	include_directories(./src/unix/linux)
	file(GLOB UV_HEADERS_UNIX_LINUX "./src/unix/linux/*.h")
	source_group(linux FILES ${UV_HEADERS_UNIX_LINUX})
	list(APPEND UV_PRIVATE_HEADERS ${UV_HEADERS_UNIX} ${UV_HEADERS_UNIX_LINUX})
elseif(WIN32)
	include_directories(./src/win)
	file(GLOB UV_HEADERS_WIN "./src/win/*.h")
	source_group(win FILES ${UV_HEADERS_WIN})
	list(APPEND UV_PRIVATE_HEADERS ${UV_HEADERS_WIN})
endif()

aux_source_directory(./src UV_SOURCES)
source_group(src FILES ${UV_SOURCES})
if(APPLE)
	aux_source_directory(./src/unix UV_SOURCES_UNIX)
	source_group(unix FILES ${UV_SOURCES_UNIX})
	aux_source_directory(./src/unix/macosx UV_SOURCES_UNIX_MACOSX)
	source_group(macosx FILES ${UV_SOURCES_UNIX_MACOSX})
	list(APPEND UV_SOURCES ${UV_SOURCES_UNIX} ${UV_SOURCES_UNIX_MACOSX})
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	aux_source_directory(./src/unix UV_SOURCES_UNIX)
	source_group(unix FILES ${UV_SOURCES_UNIX})
	aux_source_directory(./src/unix/linux UV_SOURCES_UNIX_LINUX)
	source_group(linux FILES ${UV_SOURCES_UNIX_LINUX})
	list(APPEND UV_SOURCES ${UV_SOURCES_UNIX} ${UV_SOURCES_UNIX_LINUX})
elseif(WIN32)
	aux_source_directory(./src/win UV_SOURCES_WIN)
	source_group(win FILES ${UV_SOURCES_WIN})
	list(APPEND UV_SOURCES ${UV_SOURCES_WIN})
endif()

# dynamic load library  .so .bundle
add_library(${PROJECT_NAME} MODULE ${UVWRAP_MOD_SRC} ${UVWRAP_HEADERS} ${UV_SOURCES} ${UV_PUBLIC_HEADERS} ${UV_PUBLIC_UV_HEADERS} ${UV_PRIVATE_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES
	FOLDER "cmod"
	# OUTPUT_NAME ${PROJECT_NAME}
	# VERSION "0.1.0"
	# SOVERSION "0.1.0"
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
	POSITION_INDEPENDENT_CODE ON
)

if(APPLE)
	target_link_libraries(${PROJECT_NAME} liblua)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(${PROJECT_NAME} liblua pthread dl rt)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} liblua advapi32 iphlpapi psapi shell32 user32 userenv ws2_32)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
endif()

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib/lua/5.3
	ARCHIVE DESTINATION lib/lua/5.3
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION runner/cmods)

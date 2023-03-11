cmake_minimum_required(VERSION 3.0)
project(nfd
	VERSION 1.1.6
	# DESCRIPTION "Native File Dialogue module"
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
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -Wstrict-prototypes -Wno-unused-parameter -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_DARWIN_UNLIMITED_SELECT=1 -D_DARWIN_USE_64_BIT_INODE=1") # -fvisibility=hidden 
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -Wstrict-prototypes -Wno-unused-parameter -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE -D_POSIX_C_SOURCE=200112") # -fvisibility=hidden 
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "/DLUA_BUILD_AS_DLL /DWIN32_LEAN_AND_MEAN /D_WIN32_WINNT=0x0600 /D_CRT_SECURE_NO_WARNINGS") # /Wall
	set(CMAKE_C_FLAGS_DEBUG   "/ZI /Od")
	set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")

	set(CMAKE_CXX_FLAGS         "/DLUA_BUILD_AS_DLL /DWIN32_LEAN_AND_MEAN /D_WIN32_WINNT=0x0600 /D_CRT_SECURE_NO_WARNINGS") # /Wall
	set(CMAKE_CXX_FLAGS_DEBUG   "/ZI /Od")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

set(
	EXTERNAL_INCLUDE_DIRS
	../../liblua/include
	../../liblua/core
)
set(
	MODULE_DIRS
	include
	src
	wrap
)
set(
	NOT_INCLUDE_DIR
)
if(APPLE)
	list(APPEND MODULE_DIRS src/macosx)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	list(APPEND MODULE_DIRS src/linux)
elseif(WIN32)
	list(APPEND MODULE_DIRS src/win)
endif()

set(MODULE_SRC)
set(MODULE_HEADERS)

foreach(SRC_DIR ${MODULE_DIRS} ${EXTERNAL_INCLUDE_DIRS})
	include_directories(${SRC_DIR})
endforeach(SRC_DIR)
foreach(SRC_DIR ${MODULE_DIRS} ${NOT_INCLUDE_DIR})
	file(TO_NATIVE_PATH ${SRC_DIR} NATIVE_SRC_DIR)
	set(_SRC_)
	aux_source_directory(${SRC_DIR} _SRC_)
	set(_HEADERS_)
	file(GLOB _HEADERS_ "${SRC_DIR}/*.h")
	source_group(${NATIVE_SRC_DIR} FILES ${_SRC_} ${_HEADERS_})
	list(APPEND MODULE_SRC ${_SRC_})
	list(APPEND MODULE_HEADERS ${_HEADERS_})
endforeach(SRC_DIR)

# dynamic load library  .so .bundle
add_library(${PROJECT_NAME} MODULE ${MODULE_SRC} ${MODULE_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES
	FOLDER "cmod"
	# OUTPUT_NAME ${PROJECT_NAME}
	# VERSION "0.1.0"
	# SOVERSION "0.1.0"
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
	POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(${PROJECT_NAME} liblua)
if(APPLE)
	target_link_libraries(${PROJECT_NAME}
		"-framework Foundation"
		"-framework AppKit"
	)
	if (CMAKE_HOST_SYSTEM_VERSION VERSION_GREATER_EQUAL 21) # MacOSX 12.0 is Darwin-21.0.1
		target_link_libraries(${PROJECT_NAME}
			"-framework UniformTypeIdentifiers"
		)
	endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	# find_package(GTK3 REQUIRED) # require gtk3-config.cmake
	find_package(PkgConfig) # require pkg-config
	pkg_search_module(GTK3 REQUIRED gtk+-3.0)
	include_directories(${GTK3_INCLUDE_DIRS})
	link_directories(${GTK3_LIBRARY_DIRS})
	target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} ole32 uuid)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
endif()

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION cmods/5.3
	ARCHIVE DESTINATION cmods/5.3
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION runner/cmods)

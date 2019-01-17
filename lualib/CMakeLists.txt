cmake_minimum_required(VERSION 3.0)
project(lualib
	VERSION 5.3.5
	# DESCRIPTION "Lua dynamic library"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# -DLUA_COMPAT_5_1 -DLUA_COMPAT_5_2 -DLUA_COMPAT_MODULE
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
include_directories(./include)

aux_source_directory(./core LUA_CORE_SRC)
aux_source_directory(./libraries LUA_LIBRARIES_SRC)
file(GLOB LUA_HEADERS "../include/*.h")
file(GLOB LUA_CORE_HEADERS "./include/*.h")

source_group(core FILES ${LUA_CORE_SRC})
source_group(libraries FILES ${LUA_LIBRARIES_SRC})
source_group(include FILES ${LUA_HEADERS})
source_group(coreinc FILES ${LUA_CORE_HEADERS})

add_library(${PROJECT_NAME} SHARED 
	${LUA_CORE_SRC}
	${LUA_LIBRARIES_SRC}
	${LUA_HEADERS}
	${LUA_CORE_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME ${PROJECT_NAME}
	VERSION ${PROJECT_VERSION}
	SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
	POSITION_INDEPENDENT_CODE ON)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(${PROJECT_NAME} m dl)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

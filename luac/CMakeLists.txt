cmake_minimum_required(VERSION 3.0)
project(luac
	VERSION 5.3.5
	DESCRIPTION "Lua compiler cmd"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
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
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories(../include)

aux_source_directory(../lualib/src LUA_LIB_SRC)
file(GLOB LUA_HEADERS "../include/*.h")
source_group(src FILES ${LUA_LIB_SRC})
source_group(header FILES ${LUA_HEADERS})
add_library(lualibstatic STATIC ${LUA_LIB_SRC} ${LUA_HEADERS})
set_target_properties(lualibstatic PROPERTIES
	OUTPUT_NAME "lualib"
	VERSION "5.3.5"
	SOVERSION "5.3"
	POSITION_INDEPENDENT_CODE OFF)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(lualibstatic m dl)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

aux_source_directory(./src LUAC_SRC)
source_group(src FILES ${LUAC_SRC})
add_executable(${PROJECT_NAME} ${LUAC_SRC})
target_link_libraries(${PROJECT_NAME} lualibstatic)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET lualibstatic PROPERTY FOLDER "luac")
set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "luac")

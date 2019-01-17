cmake_minimum_required(VERSION 3.0)
project(luacstruct
	VERSION 0.3
	# DESCRIPTION "Lua c struct module"
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
include_directories(../../lualib/include)

aux_source_directory(./src LUACSTRUCTMOD_SRC)

source_group(src FILES ${LUACSTRUCTMOD_SRC})

add_library(${PROJECT_NAME} MODULE ${LUACSTRUCTMOD_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	# OUTPUT_NAME ${PROJECT_NAME}
	# VERSION "0.3"
	# SOVERSION "0.3"
	POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${PROJECT_NAME} lualib)

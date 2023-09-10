cmake_minimum_required(VERSION 3.6)
project(liblua
	VERSION 5.3.5
	# DESCRIPTION "Lua dynamic library"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
# message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# set(CMAKE_C_VISIBILITY_PRESET hidden) # ignore "extern", hide all symbols

# -DLUA_COMPAT_5_1 -DLUA_COMPAT_5_2 -DLUA_COMPAT_MODULE
# LUA_USE_MACOSX and LUA_USE_LINUX are only available for lua.c and liblua, not for user
# LUA_BUILD_AS_DLL are for all windows components, include liblua, cmod library, and user exe which use liblua
if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -DLUA_USE_MACOSX")
	set(CMAKE_C_FLAGS_DEBUG   "-g -DLUA_DEBUG") # LUA_DEBUG marco for ltable.c
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -DLUA_USE_LINUX")
	set(CMAKE_C_FLAGS_DEBUG   "-g -DLUA_DEBUG")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "/DLUA_BUILD_AS_DLL") # /Wall
	set(CMAKE_C_FLAGS_DEBUG   "/ZI /Od /DLUA_DEBUG")
	set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

include_directories(./include)
include_directories(./core)

aux_source_directory(./core LUA_CORE_SRC)
aux_source_directory(./libraries LUA_LIBRARIES_SRC)
aux_source_directory(./util LUA_UTIL_SRC)
file(GLOB LUA_HEADERS "./include/*.h")
file(GLOB LUA_CORE_HEADERS "./core/*.h")

source_group(core FILES ${LUA_CORE_SRC} ${LUA_CORE_HEADERS})
source_group(libraries FILES ${LUA_LIBRARIES_SRC})
source_group(util FILES ${LUA_UTIL_SRC})
source_group(include FILES ${LUA_HEADERS})

set(LIB_LUA_ALL_SRC
	${LUA_CORE_SRC}
	${LUA_LIBRARIES_SRC}
	${LUA_UTIL_SRC}
	${LUA_HEADERS}
	${LUA_CORE_HEADERS}
)

add_library(${PROJECT_NAME} SHARED ${LIB_LUA_ALL_SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "lua"
	VERSION ${PROJECT_VERSION}
	SOVERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
	POSITION_INDEPENDENT_CODE ON
)

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
endif(WIN32)

# API check for debug
# add_definitions(-DLUA_USE_APICHECK)
# target_compile_definitions(${PROJECT_NAME} PRIVATE LUA_USE_APICHECK)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(${PROJECT_NAME} m dl)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)
install(FILES ${LUA_HEADERS} DESTINATION include/lua)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION runner
	LIBRARY DESTINATION runner/lib
)

add_subdirectory(test)

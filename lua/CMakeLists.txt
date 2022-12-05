cmake_minimum_required(VERSION 3.0)
project(lua
	VERSION 5.3.5
	# DESCRIPTION "Lua cmd"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
# message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# LUA_USE_MACOSX and LUA_USE_LINUX are only available for lua.c and liblua, not for user
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
include_directories(../liblua/core) # for lprefix.h

aux_source_directory(./src LUA_SRC)
source_group(src FILES ${LUA_SRC})

add_executable(${PROJECT_NAME} ${LUA_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
)
target_link_libraries(${PROJECT_NAME} liblua)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} readline)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME} m readline dl)
endif()

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

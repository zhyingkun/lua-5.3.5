cmake_minimum_required(VERSION 3.0)

project(lua-5.3.5
	VERSION 5.3.5
	# DESCRIPTION "Lua project" # cmake version >= 3.9
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

# The name of the top level project.
message(STATUS "Top level project is ${CMAKE_PROJECT_NAME}")
message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
message(STATUS "Current OS is ${CMAKE_SYSTEM}")
# if(NOT CMAKE_BUILD_TYPE)
# 	set(CMAKE_BUILD_TYPE "Debug")
# endif()
# message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_SKIP_INSTALL_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FLASE)
# set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib") # must before add_executable

add_subdirectory(liblua)
add_subdirectory(lua)
add_subdirectory(luac)
add_subdirectory(cmod)
# add_subdirectory(luawithlib)
add_subdirectory(demo)
add_subdirectory(tools)
add_subdirectory(lmod)

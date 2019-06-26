cmake_minimum_required(VERSION 3.0)
project(lua-5.3.5
	VERSION 5.3.5
	# DESCRIPTION "Lua project" # cmake version >= 3.9
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)

message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
message(STATUS "Current OS is ${CMAKE_SYSTEM}")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_subdirectory(lualib)
add_subdirectory(lua)
add_subdirectory(luac)
add_subdirectory(cmod)
# add_subdirectory(luawithlib)
add_subdirectory(demo)
add_subdirectory(tools)

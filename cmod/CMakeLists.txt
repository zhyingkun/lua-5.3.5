cmake_minimum_required(VERSION 3.0)
project(cmod
	VERSION 1.0.0
	# DESCRIPTION "Lua c module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)

message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")

add_subdirectory(hello)
add_subdirectory(luasocket)
# luanet must static link with lualib
# add_subdirectory(luanet)
# set_property(TARGET luanet PROPERTY FOLDER "cmod")

cmake_minimum_required(VERSION 3.0)
project(cmod
	VERSION 1.0.0
	# DESCRIPTION "Lua c module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)

message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(hello)
set_property(TARGET hello PROPERTY FOLDER "cmod")
add_subdirectory(luasocket)
set_property(TARGET luasocket PROPERTY FOLDER "cmod")
# luanet must static link with lualib
# add_subdirectory(luanet)
# set_property(TARGET luanet PROPERTY FOLDER "cmod")

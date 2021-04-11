cmake_minimum_required(VERSION 3.0)
project(lmod
	VERSION 1.0.0
	# DESCRIPTION "Lua module"
	# HOMEPAGE_URL "www.zhyingkun.com"
)

message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")

install(DIRECTORY protobuf DESTINATION mod/lua/5.3)

cmake_minimum_required(VERSION 3.0)
project(tools
	VERSION 1.0.0
	# DESCRIPTION "Demo for testing"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

add_subdirectory(luatoken)
add_subdirectory(luatt)

if(NOT WIN32)
	add_subdirectory(pbdump)
endif(NOT WIN32)

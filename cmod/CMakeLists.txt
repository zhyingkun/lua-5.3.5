cmake_minimum_required(VERSION 3.0)
project(cmod
	VERSION 1.0.0
	# DESCRIPTION "Lua c module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")

add_subdirectory(hello)
add_subdirectory(luasocket)
add_subdirectory(boolarray)
add_subdirectory(dir)
add_subdirectory(gb2312)
add_subdirectory(protobuf)
add_subdirectory(uvwrap)
add_subdirectory(json)

if(NOT WIN32)
	add_subdirectory(lproc)
endif(NOT WIN32)

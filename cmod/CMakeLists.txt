cmake_minimum_required(VERSION 3.0)
project(cmod
	VERSION 1.0.0
	# DESCRIPTION "Lua c module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")

add_subdirectory(hello)
add_subdirectory(boolarray)
if(NOT WIN32)
	add_subdirectory(lproc)
endif(NOT WIN32)

add_subdirectory(dir)
add_subdirectory(gb2312)
add_subdirectory(json)
add_subdirectory(protobuf)
add_subdirectory(luasocket)
add_subdirectory(uvwrap)
add_subdirectory(glfwrap)
add_subdirectory(bcfx)
add_subdirectory(nuklear)

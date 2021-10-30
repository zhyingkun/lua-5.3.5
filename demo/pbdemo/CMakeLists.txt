cmake_minimum_required(VERSION 3.0)
project(pbdemo
	VERSION 0.1.0
	# DESCRIPTION "Examples for protobuf"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
# message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

install_script(. decoderaw)
install_script(. decompilepb)

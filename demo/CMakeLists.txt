cmake_minimum_required(VERSION 3.0)
project(demo
	VERSION 1.0.0
	# DESCRIPTION "Demo for testing"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

add_subdirectory(c-call-lua)
add_subdirectory(c-lang)
add_subdirectory(pil)

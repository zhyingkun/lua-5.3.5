cmake_minimum_required(VERSION 3.0)
project(tools
	VERSION 1.0.0
	# DESCRIPTION "Demo for testing"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(luatt)
set_property(TARGET luatt PROPERTY FOLDER "tools")

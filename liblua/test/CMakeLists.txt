cmake_minimum_required(VERSION 3.0)
project(liblua_Test
	VERSION 1.0
	# DESCRIPTION "Lua library test"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
# message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "-Wall -DLUA_BUILD_AS_DLL")
	set(CMAKE_C_FLAGS_DEBUG   "/ZI")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories(../include)
include_directories(../core)
include_directories(./CuTest)

aux_source_directory(./src TEST_SRC)
aux_source_directory(./CuTest CUTEST_SRC)
file(GLOB CUTEST_HEADER "./CuTest/*.h")

source_group(src FILES ${TEST_SRC})
source_group(CuTest FILES ${CUTEST_SRC} ${CUTEST_HEADER})

add_executable(${PROJECT_NAME} ${TEST_SRC} ${CUTEST_SRC} ${CUTEST_HEADER})
target_link_libraries(${PROJECT_NAME} liblua)
set_target_properties(${PROJECT_NAME} PROPERTIES
	FOLDER "liblua"
)

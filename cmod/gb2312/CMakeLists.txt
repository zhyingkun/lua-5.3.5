cmake_minimum_required(VERSION 3.0)
project(gb2312
	VERSION 0.1.0
	# DESCRIPTION "Lua gb2312 module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
# message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# LUA_BUILD_AS_DLL are for all windows components, include liblua, cmod library, and user exe which use liblua
if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "/Wall /DLUA_BUILD_AS_DLL")
	set(CMAKE_C_FLAGS_DEBUG   "/ZI /Od")
	set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

include_directories(../../liblua/include)
include_directories(../../liblua/core)
include_directories(./src)
aux_source_directory(./src GB2312MOD_SRC)
file(GLOB GB2312_HEADERS "./src/*.h")
source_group(src FILES ${GB2312MOD_SRC} ${GB2312_HEADERS})
# dynamic load library  .so .bundle
add_library(${PROJECT_NAME} MODULE ${GB2312MOD_SRC} ${GB2312_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES
	FOLDER "cmod"
	# OUTPUT_NAME ${PROJECT_NAME}
	# VERSION "0.1.0"
	# SOVERSION "0.1.0"
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
	POSITION_INDEPENDENT_CODE ON
)
target_link_libraries(${PROJECT_NAME} liblua)
if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
endif(WIN32)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION cmods/5.3
	ARCHIVE DESTINATION cmods/5.3
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION runner/cmods)

aux_source_directory(./test GB2312_TEST_SRC)
source_group(src FILES ${GB2312_TEST_SRC})
add_executable(gb2312-test ${GB2312_TEST_SRC} ${GB2312MOD_SRC} ${GB2312_HEADERS})
set_target_properties(gb2312-test PROPERTIES
	FOLDER "cmod/gb2312"
)
target_link_libraries(gb2312-test liblua)

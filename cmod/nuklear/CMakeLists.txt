cmake_minimum_required(VERSION 3.0)
project(nuklear
	VERSION 4.09.0
	# DESCRIPTION "Lua nuklear module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
# message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -Wstrict-prototypes -Wno-unused-parameter") # -fvisibility=hidden 
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -Wstrict-prototypes -Wno-unused-parameter") # -fvisibility=hidden 
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "/Wall /DLUA_BUILD_AS_DLL") # /DNUKLEAR_BUILD_AS_DLL
	set(CMAKE_C_FLAGS_DEBUG   "/ZI /Od") # for debug dll in windows
	set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

set(
	EXTERNAL_INCLUDE_DIRS
	../../liblua/include
	../../liblua/core
)
set(
	MODULE_DIRS
	include
	src
	src/basic
	src/stb
	wrap
	wrap/custom
	wrap/font
)
set(
	NOT_INCLUDE_DIR
)
# if(APPLE)
# 	list(APPEND MODULE_DIRS )
# elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
# 	list(APPEND MODULE_DIRS )
# elseif(WIN32)
# 	list(APPEND MODULE_DIRS )
# endif()

set(MODULE_SRC)
set(MODULE_HEADERS)

foreach(SRC_DIR ${MODULE_DIRS} ${EXTERNAL_INCLUDE_DIRS})
	include_directories(${SRC_DIR})
endforeach(SRC_DIR)
foreach(SRC_DIR ${MODULE_DIRS} ${NOT_INCLUDE_DIR})
	file(TO_NATIVE_PATH ${SRC_DIR} NATIVE_SRC_DIR)
	set(_SRC_)
	aux_source_directory(${SRC_DIR} _SRC_)
	set(_HEADERS_)
	file(GLOB _HEADERS_ "${SRC_DIR}/*.h")
	source_group(${NATIVE_SRC_DIR} FILES ${_SRC_} ${_HEADERS_})
	list(APPEND MODULE_SRC ${_SRC_})
	list(APPEND MODULE_HEADERS ${_HEADERS_})
endforeach(SRC_DIR)

# dynamic load library  .so .bundle
add_library(${PROJECT_NAME} MODULE ${MODULE_SRC} ${MODULE_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES
	FOLDER "cmod"
	# OUTPUT_NAME ${PROJECT_NAME}
	# VERSION "0.1.0"
	# SOVERSION "0.1.0"
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
	POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(${PROJECT_NAME} liblua)
if(APPLE)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
elseif(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
endif()

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION cmods/5.3
	ARCHIVE DESTINATION cmods/5.3
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION runner/cmods)

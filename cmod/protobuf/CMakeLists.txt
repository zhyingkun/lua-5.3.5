cmake_minimum_required(VERSION 3.0)
project(protobuf
	VERSION 0.1.0
	# DESCRIPTION "Lua protobuf module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

# LUA_BUILD_AS_DLL are for all windows components, include liblua, cmod library, and user exe which use liblua
if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -fPIC -Wall")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -fPIC -Wall")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "-Wall -DLUA_BUILD_AS_DLL")
	# set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories(../../liblua/include)
include_directories(../../liblua/core)
include_directories(./src)
include_directories(./include)

aux_source_directory(./src PROTOBUFMOD_SRC)
file(GLOB PROTOBUF_HEADERS "./src/*.h")
source_group(src FILES ${PROTOBUFMOD_SRC} ${PROTOBUF_HEADERS})

file(GLOB PBC_HEADERS "./include/*.h")
source_group(include FILES ${PBC_HEADERS})

aux_source_directory(./wrap WRAP_SRC)
source_group(wrap FILES ${WRAP_SRC})

# dynamic load library  .so .bundle
add_library(${PROJECT_NAME} MODULE ${PROTOBUFMOD_SRC} ${PROTOBUF_HEADERS} ${PBC_HEADERS} ${WRAP_SRC})
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
	LIBRARY DESTINATION lib/lua/5.3
	ARCHIVE DESTINATION lib/lua/5.3
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION runner/cmods)

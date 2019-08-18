cmake_minimum_required(VERSION 3.0)
project(luac
	VERSION 5.3.5
	# DESCRIPTION "Lua compiler cmd"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories(../include)
include_directories(../lualib/include)

aux_source_directory(./src LUAC_SRC)
source_group(src FILES ${LUAC_SRC})

add_executable(${PROJECT_NAME} ${LUAC_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
target_link_libraries(${PROJECT_NAME} lualib)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

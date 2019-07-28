cmake_minimum_required(VERSION 3.0)
project(luanet
	VERSION 0.1.0
	# DESCRIPTION "Lua luanet module"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX)
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE "Debug")
endif()
message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")
message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

if(APPLE)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -DLUA_USE_MACOSX")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -DLUA_USE_LINUX")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -DLUA_BUILD_AS_DLL")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

include_directories(../../include)
include_directories(../../lualib/include)
include_directories(./include)

aux_source_directory(./src HELLOMOD_SRC)
file(GLOB LUA_NET_HEADERS "./include/*.h")

source_group(src FILES ${HELLOMOD_SRC})
source_group(include FILES ${LUA_NET_HEADERS})

# luanet must static link with lualib, not dynamic link, because of the use of luaD_rawrunprotected
add_library(${PROJECT_NAME} MODULE ${HELLOMOD_SRC} ${LUA_NET_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES
	# OUTPUT_NAME ${PROJECT_NAME}
	# VERSION "1.0.0"
	# SOVERSION "1.0.0"
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
	POSITION_INDEPENDENT_CODE ON)
target_link_libraries(${PROJECT_NAME} lualib)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

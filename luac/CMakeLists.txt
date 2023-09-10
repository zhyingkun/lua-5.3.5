cmake_minimum_required(VERSION 3.6)
project(luac
	VERSION 5.3.5
	# DESCRIPTION "Lua compiler cmd"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

if(NOT CMAKE_BUILD_TYPE)
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
	set(CMAKE_C_FLAGS         "/DLUA_BUILD_AS_DLL") # /Wall
	set(CMAKE_C_FLAGS_DEBUG   "/ZI /Od")
	set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

include_directories(../liblua/include)
include_directories(../liblua/core)

# aux_source_directory(../liblua/core LUA_CORE_SRC)
# aux_source_directory(../liblua/libraries LUA_LIBRARIES_SRC)

# set(LIBLUA_SRC ${LUA_CORE_SRC} ${LUA_LIBRARIES_SRC})
# source_group(src FILES ${LIBLUA_SRC})

# add_library(libluastatic STATIC ${LIBLUA_SRC})
# set_target_properties(libluastatic PROPERTIES
# 	FOLDER "luac"
# 	OUTPUT_NAME "lua"
# 	VERSION "5.3.5"
# 	SOVERSION "5.3"
# 	POSITION_INDEPENDENT_CODE OFF
# )

# if(APPLE)
# 	target_compile_definitions(libluastatic PRIVATE LUA_USE_MACOSX)
# elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
# 	target_compile_definitions(libluastatic PRIVATE LUA_USE_LINUX)
# 	target_link_libraries(libluastatic m dl)
# endif()


aux_source_directory(./src LUAC_SRC)
source_group(src FILES ${LUAC_SRC})

add_executable(${PROJECT_NAME} ${LUAC_SRC})
# target_link_libraries(${PROJECT_NAME} libluastatic)
# set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER "luac")
set_target_properties(${PROJECT_NAME} PROPERTIES
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
)
target_link_libraries(${PROJECT_NAME} liblua)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

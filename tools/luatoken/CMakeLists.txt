cmake_minimum_required(VERSION 3.0)
project(luatoken
	VERSION 0.1.0
	# DESCRIPTION "For lua tag type"
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
	set(CMAKE_C_FLAGS         "/Wall")
	set(CMAKE_C_FLAGS_DEBUG   "/ZI /Od")
	set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

aux_source_directory(./src TOOLS_LUATOKEN_SRC)
source_group(src FILES ${TOOLS_LUATOKEN_SRC})

add_executable(${PROJECT_NAME} ${TOOLS_LUATOKEN_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
	FOLDER "tools"
)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

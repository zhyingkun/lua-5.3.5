cmake_minimum_required(VERSION 3.6)
project(runner
	VERSION 0.1.0
	# DESCRIPTION "lua runner"
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

include_directories(../../liblua/include)
aux_source_directory(./src RUNNER_SRC)
source_group(src FILES ${RUNNER_SRC})

add_executable(${PROJECT_NAME} ${RUNNER_SRC})
target_link_libraries(${PROJECT_NAME} liblua)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "demo")
if(APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH @executable_path/lib)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH \$ORIGIN/lib)
elseif(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH ./lib)
endif()

# install(TARGETS ${PROJECT_NAME}
# 	RUNTIME DESTINATION bin
# 	LIBRARY DESTINATION lib
# 	ARCHIVE DESTINATION lib
# )
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION runner)
install(DIRECTORY scripts DESTINATION runner)

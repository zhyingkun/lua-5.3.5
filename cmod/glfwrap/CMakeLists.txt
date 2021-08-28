cmake_minimum_required(VERSION 3.0)
project(glfwrap
	VERSION 3.3.0
	# DESCRIPTION "Lua glfw module"
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
	set(CMAKE_CXX_FLAGS         "-std=c11 -Wall -Wextra -Wstrict-prototypes -Wno-unused-parameter")
	set(CMAKE_CXX_FLAGS_DEBUG   "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_CXX_FLAGS         "-std=c11 -Wall -Wextra -Wstrict-prototypes -Wno-unused-parameter")
	set(CMAKE_CXX_FLAGS_DEBUG   "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_CXX_FLAGS         "-Wall -DLUA_BUILD_AS_DLL")
	# set(CMAKE_CXX_FLAGS_DEBUG   "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

set(
	EXTERNAL_INCLUDE_DIRS
	../../liblua/include
	../../liblua/core
	./include
)
set(
	MODULE_DIRS
	include/GLFW
	src
	wrap
)

if(APPLE)
	set(_GLFW_COCOA 1)
	list(APPEND MODULE_DIRS
		config/macosx
		src/macosx
		src/posix
	)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(_GLFW_X11 1)
	list(APPEND MODULE_DIRS
		config/linux
		src/linux
		src/posix
	)
elseif(WIN32)
	set(_GLFW_WIN32 1)
	list(APPEND MODULE_DIRS
		config/windows
		src/windows
	)
endif()

set(MODULE_SRC)
set(MODULE_HEADERS)

foreach(SRC_DIR ${MODULE_DIRS} ${EXTERNAL_INCLUDE_DIRS})
	include_directories(${SRC_DIR})
endforeach(SRC_DIR)
foreach(SRC_DIR ${MODULE_DIRS})
	file(TO_NATIVE_PATH ${SRC_DIR} NATIVE_SRC_DIR)
	set(_SRC_)
	aux_source_directory(${SRC_DIR} _SRC_)
	set(_HEADERS_)
	file(GLOB _HEADERS_ "${SRC_DIR}/*.h")
	source_group(${NATIVE_SRC_DIR} FILES ${_SRC_} ${_HEADERS_})
	list(APPEND MODULE_SRC ${_SRC_})
	list(APPEND MODULE_HEADERS ${_HEADERS_})
endforeach(SRC_DIR)

if(APPLE)
	# For some reason, CMake doesn't know about .m
	set_source_files_properties(${glfw_SOURCES} PROPERTIES LANGUAGE C)
	#--------------------------------------------------------------------
	# Use Cocoa for window creation and NSOpenGL for context creation
	#--------------------------------------------------------------------
	list(APPEND glfw_LIBRARIES
		"-framework Cocoa"
		"-framework IOKit"
		"-framework CoreFoundation"
		"-framework CoreVideo"
	)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	#--------------------------------------------------------------------
	# Use X11 for window creation
	#--------------------------------------------------------------------
	find_package(X11 REQUIRED)
	# Set up library and include paths
	list(APPEND glfw_INCLUDE_DIRS "${X11_X11_INCLUDE_PATH}")
	list(APPEND glfw_LIBRARIES "${X11_X11_LIB}" "${CMAKE_THREAD_LIBS_INIT}")
	# Check for XRandR (modern resolution switching and gamma control)
	if (NOT X11_Xrandr_FOUND)
		message(FATAL_ERROR "The RandR headers were not found")
	endif()
	# Check for Xinerama (legacy multi-monitor support)
	if (NOT X11_Xinerama_FOUND)
		message(FATAL_ERROR "The Xinerama headers were not found")
	endif()
	# Check for Xkb (X keyboard extension)
	if (NOT X11_Xkb_FOUND)
		message(FATAL_ERROR "The X keyboard extension headers were not found")
	endif()
	# Check for Xcursor (cursor creation from RGBA images)
	if (NOT X11_Xcursor_FOUND)
		message(FATAL_ERROR "The Xcursor headers were not found")
	endif()
	list(APPEND glfw_INCLUDE_DIRS
		"${X11_Xrandr_INCLUDE_PATH}"
		"${X11_Xinerama_INCLUDE_PATH}"
		"${X11_Xkb_INCLUDE_PATH}"
		"${X11_Xcursor_INCLUDE_PATH}"
	)
	#--------------------------------------------------------------------
	# Find and add Unix math and time libraries
	#--------------------------------------------------------------------
	find_library(RT_LIBRARY rt)
	mark_as_advanced(RT_LIBRARY)
	if (RT_LIBRARY)
		list(APPEND glfw_LIBRARIES "${RT_LIBRARY}")
	endif()
	find_library(MATH_LIBRARY m)
	mark_as_advanced(MATH_LIBRARY)
	if (MATH_LIBRARY)
		list(APPEND glfw_LIBRARIES "${MATH_LIBRARY}")
	endif()
	if (CMAKE_DL_LIBS)
		list(APPEND glfw_LIBRARIES "${CMAKE_DL_LIBS}")
	endif()
elseif(WIN32)
endif()

set(_GLFW_BUILD_DLL 1)
add_library(${PROJECT_NAME} MODULE ${MODULE_SRC} ${MODULE_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES
	FOLDER "cmod"
	# OUTPUT_NAME "${PROJECT_NAME}"
	# VERSION "3.3"
	# SOVERSION "3"
	INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
	POSITION_INDEPENDENT_CODE ON
)
target_compile_definitions(${PROJECT_NAME} PRIVATE _GLFW_USE_CONFIG_H)
target_compile_definitions(${PROJECT_NAME} PRIVATE _GLFW_BUILD_DLL)

target_link_libraries(${PROJECT_NAME} PRIVATE liblua ${glfw_LIBRARIES})

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
endif(WIN32)

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib/lua/5.3
	ARCHIVE DESTINATION lib/lua/5.3
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION runner/cmods)

cmake_minimum_required(VERSION 3.0)
project(portaudio
	VERSION 19.7.0
	# DESCRIPTION "Lua PortAudio module"
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
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -Wstrict-prototypes -Wno-unused-parameter -DPA_USE_COREAUDIO")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	set(CMAKE_C_FLAGS         "-std=gnu99 -Wall -Wextra -Wstrict-prototypes -Wno-unused-parameter -DPA_USE_ALSA")
	set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
elseif(WIN32)
	set(CMAKE_C_FLAGS         "-Wall -DLUA_BUILD_AS_DLL -DPAWIN_USE_DIRECTSOUNDFULLDUPLEXCREATE -DPAWIN_USE_WDMKS_DEVICE_INFO")
	# set(CMAKE_C_FLAGS_DEBUG   "-g")
	set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

set(
	EXTERNAL_INCLUDE_DIRS
	../../liblua/include
	../../liblua/core
)
set(
	MODULE_DIRS
	include
	src/common
	hostapi/skeleton
	wrap
)
set(
	NOT_INCLUDE_DIR
)
if(APPLE)
	list(APPEND MODULE_DIRS src/os/unix src/hostapi/coreaudio)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	list(APPEND MODULE_DIRS src/os/unix src/hostapi/alsa)
elseif(WIN32)
	list(APPEND MODULE_DIRS
		src/os/win
		src/hostapi/dsound
		src/hostapi/wmme
		src/hostapi/wdmks
	)
endif()

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

if(WIN32)
	set_source_files_properties(${MODULE_SRC} PROPERTIES COMPILE_DEFINITIONS "UNICODE;_UNICODE")
endif(WIN32)

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

if(APPLE)
	target_link_libraries(${PROJECT_NAME} liblua
		"-framework CoreAudio"
		"-framework AudioToolbox"
		"-framework AudioUnit"
		"-framework CoreFoundation"
		"-framework CoreServices"
	)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
	target_link_libraries(${PROJECT_NAME} liblua asound pthread m)
elseif(WIN32)
	target_link_libraries(${PROJECT_NAME} liblua winmm dsound ole32 uuid setupapi)
	set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "lib")
endif()

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib/lua/5.3
	ARCHIVE DESTINATION lib/lua/5.3
)

install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION runner/cmods)

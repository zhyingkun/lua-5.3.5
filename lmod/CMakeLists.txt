cmake_minimum_required(VERSION 3.0)
project(lmod
	VERSION 1.0.0
	# DESCRIPTION "Lua module"
	# HOMEPAGE_URL "www.zhyingkun.com"
)

# message(STATUS "CMakeLists.txt for ${PROJECT_NAME}")

macro(sub_dir_list dirlist_ curdir)
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	set(dirlist)
	foreach(child ${children})
		if(IS_DIRECTORY ${curdir}/${child})
			list(APPEND dirlist ${child})
		endif()
	endforeach()
	SET(${dirlist_} ${dirlist})
endmacro()

sub_dir_list(DIRLIST ${CMAKE_CURRENT_SOURCE_DIR})

foreach(LMOD_DIR ${DIRLIST})
	install(DIRECTORY ${LMOD_DIR} DESTINATION mod/lua/5.3)
	install(DIRECTORY ${LMOD_DIR} DESTINATION runner/lmods)
endforeach(LMOD_DIR)

file(GLOB FILELIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.lua)

install(FILES ${FILELIST} DESTINATION mod/lua/5.3)
install(FILES ${FILELIST} DESTINATION runner/lmods)

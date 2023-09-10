cmake_minimum_required(VERSION 3.6)
project(tools
	VERSION 1.0.0
	# DESCRIPTION "Demo for testing"
	# HOMEPAGE_URL "www.zhyingkun.com"
	LANGUAGES C CXX
)

add_subdirectory(luatoken)
add_subdirectory(luatt)

# install(DIRECTORY dir DESTINATION tools/xxx)
# install(CODE "execute_process(COMMAND cmake -E create_symlink ${CMAKE_INSTALL_PREFIX}/tools/xxx/yyy.z ${CMAKE_INSTALL_PREFIX}/bin/yyy)")

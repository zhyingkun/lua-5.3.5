# cmake_minimum_required(VERSION 3.0)
# project(hellomod)
include_directories(../../include)
aux_source_directory(. HELLOMOD_SRC)
add_library(hellomod MODULE ${HELLOMOD_SRC})#dynamic load library  .so .bundle
target_link_libraries(hellomod lualib)

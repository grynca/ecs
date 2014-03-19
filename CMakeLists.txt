cmake_minimum_required(VERSION 2.8)

project(ecs)

# add local cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# env include and libs dir
include_directories($ENV{CMAKE_PREFIX_PATH}/include)
link_directories($ENV{CMAKE_PREFIX_PATH}/lib)

# install prefix
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "default install path" FORCE)

# enable c++11
include(EnableCXX11)

# get all headers
file(GLOB_RECURSE HEADERS "*.h")
# this will get them listed in IDE
add_custom_target( show_me_includes SOURCES ${HEADERS} )

add_subdirectory(src/ecsTest)

# install includes
install( DIRECTORY "include" DESTINATION ${CMAKE_INSTALL_PREFIX} )

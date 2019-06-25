cmake_minimum_required(VERSION 3.14)
project(Osmium)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(kernel)
add_subdirectory(lib-common)
add_subdirectory(lib-osmium)
add_subdirectory(modules/init)

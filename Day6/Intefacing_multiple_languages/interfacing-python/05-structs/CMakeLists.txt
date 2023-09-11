# CMake configuration demo

# version 3.10 is bundled with ubuntu18.04
cmake_minimum_required(VERSION 3.10)

# set the project name, version, and primary programming language (C)
project(Struct VERSION 0.1 LANGUAGES C)

option(BUILD_SHARED_LIBS "Build shared library" OFF)
if(BUILD_SHARED_LIBS)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

CFLAGS=-shared -fPIC -Wall -O

# SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
# set(CMAKE_C_FLAGS "${CFLAGS}")


# add the executable
add_executable(data_cmake.so data.c)

# CMake configuration demo
# version 3.10 is bundled with ubuntu18.04
cmake_minimum_required(VERSION 3.10)

# set the project name, version, and primary programming language (C++)
project(nonblocking VERSION 0.1 LANGUAGES C)

# look for MPI C interface
find_package(MPI REQUIRED)

# add the MPI executable
add_executable(ring_nonblocking ring_nonblocking.c)
target_link_libraries(ring_nonblocking PRIVATE MPI::MPI_C)

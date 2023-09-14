# Cmake version
cmake_minimum_required(VERSION 3.10)

# project infor
project(ireduce VERSION 0.1 LANGUAGES C)

# mpi
find_package(MPI REQUIRED)

# add MPI executable
add_executable(ireduce ireduce.c)
target_link_libraries(ireduce PRIVATE MPI::MPI_C)
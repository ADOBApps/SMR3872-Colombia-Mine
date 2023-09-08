# Compile

## Load mpi module
- module load cineca-hpyc

## OpenMP
- gcc -o ljmd_type  ljmd_type.c -fopenmp -lm

## MPI
- mpicc -Wall -g -O3 -ffast-math -fomit-frame-pointer -lm -DLJMD_VERSION=1.0 -o ljmd-mpi.x ljmd-mpi.c


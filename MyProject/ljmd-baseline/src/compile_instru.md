# Compile

## Load mpi module
module load cineca-hpyc

# Backup
gcc -o ljmd-backup  ljmd-backup.c -lm -DLJMD_VERSION=1.0

# Serial
gcc -o ljmd-serial  ljmd.c -lm -DLJMD_VERSION=1.0

## OpenMP
gcc -o ljmd-type  ljmd-type.c -fopenmp -lm -DLJMD_VERSION=1.0

## MPI
mpicc -Wall -g -O3 -ffast-math -fomit-frame-pointer -lm -DLJMD_VERSION=1.0 -o ljmd-mpi.x ljmd-mpi.c


#!/bin/bash
#SBATCH -A ICT23_SMR3872
#SBATCH -p boost_usr_prod
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=32
#SBATCH --cpus-per-task=1
#SBATCH --time 00:10:00
#SBATCH --gres=gpu:1
#SBATCH --mem=490000MB

#modulefiles to be loaded to have MPI on Leonardo
module load cineca-hpyc;

#Compiling
cmake ..;
make;

mpirun -np 4 mpi_bcast
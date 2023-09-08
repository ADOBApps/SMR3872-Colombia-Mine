#!/bin/bash
#SBATCH -A ICT23_SMR3872
#SBATCH -p boost_usr_prod
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=32
#SBATCH --time 00:10:00
#SBATCH --gres=gpu:1
#SBATCH --mem=490000MB

#modulefiles to be loaded to have MPI on Leonardo
module purge
module load openmpi/4.1.4--gcc--11.3.0-cuda-11.8

gcc -o my_pi.x my_pi.c -fopenmp

./my_pi.x

export OMP_NUM_THREADS=16
./my_pi.x
export OMP_NUM_THREADS=8
./my_pi.x
export OMP_NUM_THREADS=4
./my_pi.x
export OMP_NUM_THREADS=2
./my_pi.x
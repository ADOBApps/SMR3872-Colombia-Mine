#!/bin/bash
#SBATCH -A ICT23_SMR3872
#SBATCH -p boost_usr_prod
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=32
#SBATCH --cpus-per-task=1
#SBATCH --time 00:10:00
#SBATCH --gres=gpu:1
#SBATCH --mem=490000MB

# Remove output files
# rm slurm*.out;

#modulefiles to be loaded to have MPI on Leonardo
module load cineca-hpyc;

cd examples;

echo "Test for backup"
../ljmd-backup.x < argon_108.inp

echo "";
echo "=============================================";
echo "Test for serial"
../ljmd-serial.x < argon_108.inp

{ # try
    echo "";
    echo "=============================================";
    echo "Test for MPI"
    mpirun -np 8 ../ljmd-mpi.x < argon_108.inp 
} || { # catch
    echo "MPI no ready yet";
}

# { # try } || { # catch }
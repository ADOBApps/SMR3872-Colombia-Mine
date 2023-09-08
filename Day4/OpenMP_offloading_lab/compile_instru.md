# Allow GPU Acceleration

## Load mpi module
```
module load cineca-hpyc
```

```
srun -A ICT23_SMR3872 -N 1 -p boost_usr_prod --gres gpu:1 --pty bash
```

```
module load nvhpc/23.1
```

```
nvc -mp=gpu -gpu=cc80 -O3 -o SAXPY_type.x saxpy_type.c 
```

# Compile

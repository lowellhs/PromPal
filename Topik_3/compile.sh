#!/bin/bash

n=$1

echo "#define n ${n}" > init_jacobi.c
echo "#define limit_iter 1e+4" >> init_jacobi.c
echo "#define TOL 1e-6" >> init_jacobi.c
echo >> init_jacobi.c

mpicc -o jacobi/sequential.o jacobi/sequential.c -lm
mpicc -o jacobi/mpi_nonopt.o jacobi/mpi_nonopt.c -lm

#!/bin/bash

nvcc -lcublas -o matrixMultiplication_cublas.o matrixMultiplication_cublas.cu
nvcc -o matrixMultiplication_nonshared.o matrixMultiplication_nonshared.cu
nvcc -o matrixMultiplication_shared.o matrixMultiplication_shared.cu
gcc -o matrixMultiplication_sequential.o matrixMultiplication_sequential.c
mpicc -o matrixMultiplication_mpi.o matrixMultiplication_mpi.c

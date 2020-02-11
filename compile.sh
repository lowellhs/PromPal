#!/bin/bash

mpicc -o mpi_matmat_row.o mpi_matmat_row.c
mpicc -o mpi_matvec_row.o mpi_matvec_row.c
mpicc -o mpi_matvec_col.o mpi_matvec_col.c

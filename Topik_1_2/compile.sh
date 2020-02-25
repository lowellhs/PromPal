#!/bin/bash

d1=point_to_point
d2=sequential
d3=collective

mpicc -o $d1/mpi_matmat_row.o $d1/mpi_matmat_row.c -lm
mpicc -o $d1/mpi_matvec_row.o $d1/mpi_matvec_row.c -lm
mpicc -o $d1/mpi_matvec_col.o $d1/mpi_matvec_col.c -lm
mpicc -o $d2/seq_matvec.o $d2/seq_matvec.c -lm
mpicc -o $d2/seq_matmat.o $d2/seq_matmat.c -lm
mpicc -o $d3/mpi_matvec_row.o $d3/mpi_matvec_row.c -lm
mpicc -o $d3/mpi_matvec_col.o $d3/mpi_matvec_col.c -lm
mpicc -o $d3/mpi_matmat_row.o $d3/mpi_matmat_row.c -lm

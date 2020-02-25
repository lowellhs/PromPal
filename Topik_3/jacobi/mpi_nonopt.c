#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"

#define n 2
#define limit_iter 100
#define TOL 1e-6

double A[n][n], b[n], x_iter[n];

int main(int argc, char **argv) {
  int num_procs, my_rank, rows;
  int i, j, k;
  double temp;

  k = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (my_rank == 0) {
    A[0][0] = 1;
    A[0][1] = 1;
    A[1][0] = 1;
    A[1][1] = -1;
    b[0] = 10;
    b[1] = 8;
    x_iter[0] = 8;
    x_iter[1] = 3;
  }

  rows = n / num_procs;
  double recA[rows][n];
  double recb[rows], sendx[n], x_iter_old[n];
  double dist;

  MPI_Bcast(&x_iter, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(&A, rows*n, MPI_DOUBLE, &recA, rows*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(&b, rows, MPI_DOUBLE, &recb, rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  do {
    k++;
    for (i=0; i<n; i++) {
      x_iter_old[i] = x_iter[i];
    }
    for (i=0; i<rows; i++) {
      temp = 0.0;
      for (j=0; j<n; j++) {
        if (j != i) {
          temp = temp + recA[i][j] * x_iter[j];
        }
      }
      sendx[my_rank*rows+i] = (recb[i] - temp) / recA[i][i];
    }
    MPI_Allgather(&sendx, rows, MPI_DOUBLE, &x_iter, rows, MPI_DOUBLE, MPI_COMM_WORLD);
    dist = norm_vector(n, x_iter, x_iter_old);
  } while (k < limit_iter && dist > TOL);

  MPI_Finalize();
  return 0;
}
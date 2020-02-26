#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init_jacobi.c"

double A[n][n], b[n], x_iter[n];

int main(int argc, char **argv) {
  int num_procs, my_rank, rows, from_file, print_flag, i, j, rc, k;
  double temp, start, stop;

  k = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  if (from_file) {
    read_matrix(n, n, A, argv[3], " ");
    read_vector(n, b, argv[4], " ");
  } else {
    init_matrix_i(n, A);
    init_vector(n, b);
  }
  for (i=0; i<n; i++) {
	x_iter[i] = 0;
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
	
  if (my_rank == 0) {
    if (!print_flag) {
      print_vector(n, x_iter);
    } else {
      char *str_a = "TOTAL TIME : %.9f s, iteration count %d\n";
      printf(str_a, stop - start, k);
    }
  }

  return 0;
}
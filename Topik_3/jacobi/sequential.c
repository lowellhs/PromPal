#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init_jacobi.c"

double A[n][n], b[n], x_iter[n];

int main(int argc, char **argv) {
  int num_procs, my_rank, from_file, print_flag, i, j, rc, k;
  double temp, start, stop;

  k = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (num_procs > 1) {
    MPI_Abort(MPI_COMM_WORLD, rc);
    exit(1);
  }

  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  if (from_file) {
    read_matrix(n, n, A, argv[3], " ");
    read_vector(n, b, argv[4], " ");
    for (i=0; i<n; i++) {
      x_iter[i] = 0;
    }
  } else {
    init_matrix_i(n, A);
    init_vector(n, b);
    for (i=0; i<n; i++) {
      x_iter[i] = 0;
    }
  }
  
  print_matrix(n, n, A);
  print_vector(n, b);

  start = MPI_Wtime();
  double x_iter_old[n];
  double dist;
  do {
    k++;
    for (i=0; i<n; i++) {
      x_iter_old[i] = x_iter[i];
      temp = 0.0;
      for (j=0; j<n; j++) {
        if (j != i) {
          temp = temp + A[i][j] * x_iter[j];
        }
      }
      x_iter[i] = (b[i] - temp) / A[i][i];
    }
    dist = norm_vector(n, x_iter, x_iter_old);
  } while (k < limit_iter && dist > TOL);
  stop = MPI_Wtime();
  MPI_Finalize();

  if (!print_flag) {
    print_vector(n, x_iter);
  } else {
    char *str_a = "TOTAL TIME : %.9f s, iteration count %d\n";
    printf(str_a, stop - start, k);
  }

  return 0;
}
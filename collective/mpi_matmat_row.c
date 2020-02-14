#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init.c"

MPI_Status status;
double A[n][n], B[n][n], C[n][n];

int main(int argc, char **argv) {
  int num_procs, my_rank, num_workers, rows_per_tasks, rem_rows;
  int i, j, k, from_file, print_flag;
  struct timeval starts[2], stops[2];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  if (my_rank == 0) {
    if (from_file) {
      read_matrix(m, n, A, argv[3], " ");
      read_vector(n, x, argv[4], " ");
    } else {
      printf("Perkalian matriks (%dx%d) vektor (%dx1) I . x = b\n...\n", n, n, n);
      init_matrix_i(n, A);
      init_vector(n, x);
    }
  }

  gettimeofday(&starts[0], 0);
  rows_per_tasks = m / num_procs;
  rem_rows = m % num_procs;
  double recA[rows_per_tasks][n];
  double sendb[rows_per_tasks];

  gettimeofday(&starts[1], 0);
  MPI_Bcast(&x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(&A[0][0], rows_per_tasks*n, MPI_DOUBLE, &recA, rows_per_tasks*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  for (i=0; i<rows_per_tasks; i++) {
    sendb[i] = 0.0;
    for (j=0; j<n; j++) {
      sendb[i] = sendb[i] + recA[i][j] * x[j];
    }
  }

  MPI_Gather(&sendb, rows_per_tasks, MPI_DOUBLE, &b, rows_per_tasks, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  gettimeofday(&stops[1], 0);
  if (my_rank == 0) {
    for (i=rows_per_tasks*num_procs; i<m; i++) {
      b[i] = 0.0;
      for (j=0; j<n; j++) {
        b[i] = b[i] + A[i][j] * x[j];
      }
    }
  }
  gettimeofday(&stops[0], 0);
  if (my_rank == 0) {
    if (print_flag == 1) {
      print_vector(m, b);
    }
    if (from_file == 0) {
      char *equals_x_b = vector_equals(n, x, b) ? "sama" : "berbeda";
      printf("Perkalian I . x = b menghasilkan b %s dengan x\n", equals_x_b);
    }
    printf("Total time is %.9f s\n", time_elapsed(stops[0], starts[0]));
    printf("Total communication time is %.9f s\n", time_elapsed(stops[1], starts[1]));
  }

  MPI_Finalize();
  return 0;
}
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init_jacobi.c"

double A[n][n], b[n], x_iter[n];

int main(int argc, char **argv) {
  int num_procs, my_rank, rows, from_file, print_flag, i, j, k, i_start, i_end;
  double start, stop, start_a, stop_a, start_b, stop_b, total_comm;

  k = 0;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  if (my_rank == 0) {
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
  }

  total_comm = 0.0;
  start = MPI_Wtime();
  rows = n / num_procs;
  i_start = my_rank * rows;
  i_end = i_start + rows;
  double *recAdata = (double *)malloc(sizeof(double)*rows*n);
  double **recA = (double **)malloc(sizeof(double *)*rows);
  for (i=0; i<rows; i++) {
    recA[i] = &(recAdata[i*n]);
  }
  double recb[rows], sendx[rows], x_iter_new[n];
  double dist;

  start_a = MPI_Wtime();
  MPI_Bcast(&x_iter, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(&A, rows*n, MPI_DOUBLE, &(recA[0][0]), rows*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(&b, rows, MPI_DOUBLE, &recb, rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  stop_a = MPI_Wtime();
  total_comm += (stop_a - start_a);

  do {
    k++;
    for (i=0; i<rows; i++) {
      sendx[i] = recb[i];
      for (j=0; j<n; j++) {
        if ((my_rank*rows+i) != j) {
          sendx[i] -= (recA[i][j] * x_iter[j]);
        }
      }
      sendx[i] /= recA[i][my_rank*rows+i];
    }
    start_b = MPI_Wtime();
    MPI_Allgather(&sendx, rows, MPI_DOUBLE, &x_iter_new, rows, MPI_DOUBLE, MPI_COMM_WORLD);
    stop_b = MPI_Wtime();
    total_comm += (stop_b - start_b);
    dist = norm_vector(n, x_iter_new, x_iter);
    for (i=0; i<n; i++) {
      x_iter[i] = x_iter_new[i];
    }
  } while (k < limit_iter && dist > TOL);
  stop = MPI_Wtime();
  MPI_Finalize();
	
  if (print_flag) {
    printf("Processor %d, communication time : %.9f s\n", my_rank, total_comm);
  }

  if (my_rank == 0) {
    if (!print_flag) {
      print_vector(n, x_iter);
    } else {
      char *str_a = "TOTAL TIME : %.9f s, iteration count=%d, error=%.9f\n";
      printf(str_a, stop - start, k, dist);
    }
  }

  return 0;
}
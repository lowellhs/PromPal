#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init.c"

MPI_Status status;
double A[m][n], x[n], b[m];

int main(int argc, char **argv) {
  int num_procs, my_rank, num_workers, rows;
  int i, j, from_file, print_flag;
  double start, start_node, start_calc, start_comm_1, start_comm_2;
  double stop, stop_node, stop_calc, stop_comm_1, stop_comm_2;

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
      if (print_flag) {
        printf("Perkalian matriks (%dx%d) vektor (%dx1) I . x = b\n...\n", n, n, n);
      }
      init_matrix_i(n, A);
      init_vector(n, x);
    }
  }

  start = MPI_Wtime();
  rows = m / num_procs;
  if (my_rank == num_procs-1) {
    rows = rows + (m % num_procs);
  }
  double recA[rows][n];
  double sendb[rows];

  start_comm_1 = MPI_Wtime();
  MPI_Bcast(&x, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(&A, rows*n, MPI_DOUBLE, &recA, rows*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  stop_comm_1 = MPI_Wtime();

  start_calc = MPI_Wtime();
  for (i=0; i<rows; i++) {
    sendb[i] = 0.0;
    for (j=0; j<n; j++) {
      sendb[i] = sendb[i] + recA[i][j] * x[j];
    }
  }
  stop_calc = MPI_Wtime();

  start_comm_2 = MPI_Wtime();
  MPI_Gather(&sendb, rows, MPI_DOUBLE, &b, rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  stop_comm_2 = MPI_Wtime();
  stop = MPI_Wtime();

  if (print_flag) {
    char *ptr = "Process %d, calculation: %.9f s, Bcast-Scatter: %.9f s, Gather: %.9f s\n";
    printf(ptr, my_rank, stop_calc-start_calc, stop_comm_1-start_comm_1, my_rank, stop_comm_2-start_comm_2);
  }
  MPI_Finalize();

  if (my_rank == 0) {
    if (!print_flag) {
      print_vector(m, b);
    } else {
      if (!from_file) {
        char *equals_x_b = vector_equals(n, x, b) ? "sama" : "berbeda";
        printf("Perkalian I . x = b menghasilkan b %s dengan x\n", equals_x_b);
      }
      printf("Total time is %.9f s\n", stop - start);
    }
  }
  return 0;
}
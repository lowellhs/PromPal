#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init.c"

MPI_Status status;
double A[n][n], B[n][n], C[n][n];

int main(int argc, char **argv) {
  int num_procs, my_rank, num_workers, rows;
  int i, j, k, from_file, print_flag;
  double start, start_node, start_calc, start_comm_1, start_comm_2;
  double stop, stop_node, stop_calc, stop_comm_1, stop_comm_2;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  if (my_rank == 0) {
    if (from_file) {
      read_matrix(n, n, A, argv[3], " ");
      read_matrix(n, n, B, argv[4], " ");
    } else {
      if (print_flag) {
        printf("Perkalian matriks (%dx%d) A . I = C\n---\n", n, n);
      }
      init_matrix(n, n, A);
      init_matrix_i(n, B);
    }
  }

  start = MPI_Wtime();
  rows = m / num_procs;
  double recA[rows][n];
  double sendC[rows][n];

  start_comm_1 = MPI_Wtime();
  MPI_Bcast(&B, n*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(&A, rows*n, MPI_DOUBLE, &recA, rows*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  stop_comm_1 = MPI_Wtime();

  start_calc = MPI_Wtime();
  for (k=0; k<rows; k++) {
    for (j=0; j<n; j++) {
      sendC[k][j] = 0;
      for (i=0; i<n; i++) {
        sendC[k][j] = sendC[k][j] + recA[k][i] * B[i][j];
      }
    }
  }
  if (my_rank == 0) {
    for (k=rows*num_procs; k<m; k++) {
      for (j=0; j<n; j++) {
        C[k][j] = 0;
        for (i=0; i<n; i++) {
          C[k][j] = C[k][j] + A[k][i] * B[i][j];
        }
      }
    }
  }
  stop_calc = MPI_Wtime();

  start_comm_2 = MPI_Wtime();
  MPI_Gather(&sendC, rows*n, MPI_DOUBLE, &C, rows*n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  stop_comm_2 = MPI_Wtime();
  stop = MPI_Wtime();

  if (print_flag) {
    char *ptr = "Process %d, calculation: %.9f s, Bcast-Scatter: %.9f s, Gather: %.9f s\n";
    printf(ptr, my_rank, stop_calc-start_calc, stop_comm_1-start_comm_1, my_rank, stop_comm_2-start_comm_2);
  }
  MPI_Finalize();

  if (my_rank == 0) {
    if (!print_flag) {
      print_matrix(n, n, C);
    } else {
      if (!from_file) {
        char *equals_A_C = matrix_equals(n, n, A, C) ? "sama" : "berbeda";
        printf("Perkalian A . I = C menghasilkan C %s dengan A\n", equals_A_C);
      }
      printf("Total time is %.9f s\n", stop - start);
    }
  }
  return 0;
}
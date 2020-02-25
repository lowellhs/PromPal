#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init.c"

double A[n][n], B[n][n], C[n][n];

int main(int argc, char **argv) {
  int i, j, k, rc, print_flag, from_file, num_procs;
  double start, stop;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  if (num_procs > 1) {
    MPI_Abort(MPI_COMM_WORLD, rc);
    exit(1);
  }

  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  if (from_file) {
      read_matrix(n, n, A, argv[3], " ");
      read_matrix(n, n, B, argv[4], " ");
  } else {
      if (print_flag) {
        printf("Perkalian matriks (%dx%d) A . I = C\n", n, n);
      }
      init_matrix(n, n, A);
      init_matrix_i(n, B);
  }

  start = MPI_Wtime();
  for (k=0; k<n; k++) {
    for (j=0; j<n; j++) {
      C[k][j] = 0;
      for (i=0; i<n; i++) {
        C[k][j] = C[k][j] + A[k][i] * B[i][j];
      }
    }
  }
  stop = MPI_Wtime();
  MPI_Finalize();

  if (!print_flag) {
    print_matrix(n, n, C);
  } else {
    char *str_a = "TOTAL TIME : %.9f s\n";
    printf(str_a, stop - start);
  }

  return 0;
}
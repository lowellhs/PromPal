#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init.c"

double A[m][n], x[n], b[m];

int main(int argc, char **argv) {
  int i, j, rc, print_flag, from_file, num_procs;
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
    read_matrix(m, n, A, argv[3], " ");
    read_vector(n, x, argv[4], " ");
  } else {
      if (print_flag) {
        printf("Perkalian matriks (%dx%d) vektor (%dx1) I . x = b\n...\n", n, n, n);
      }
      init_matrix_i(n, A);
      init_vector(n, x);
  }

  start = MPI_Wtime();
  for (i=0; i<m; i++) {
    b[i] = 0.0;
    for (j=0; j<n; j++) {
      b[i] = b[i] + A[i][j] * x[j];
    }
  }
  stop = MPI_Wtime();
  MPI_Finalize();

  if (!print_flag) {
    print_vector(m, b);
  } else {
    char *str_a = "Total time is %.9f s\n";
    printf(str_a, stop - start);
  }

  return 0;
}
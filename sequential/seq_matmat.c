#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../helper.c"
#include "../init.c"

double A[n][n], B[n][n], C[n][n];

int main(int argc, char **argv) {
  int i, j, k, print_flag, from_file;
  struct timeval start, stop;

  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  if (from_file) {
      read_matrix(n, n, A, argv[3], " ");
      read_matrix(n, n, B, argv[4], " ");
  } else {
      printf("Perkalian matriks (%dx%d) A . I = C\n...\n", n, n);
      init_matrix(n, n, A);
      init_matrix_i(n, B);
  }

  gettimeofday(&start, 0);
  for (k=0; k<n; k++) {
    for (j=0; j<n; j++) {
      C[k][j] = 0;
      for (i=0; i<n; i++) {
        C[k][j] = C[k][j] + A[k][i] * B[i][j];
      }
    }
  }
  gettimeofday(&stop, 0);

  if (print_flag == 1) {
    print_matrix(n, n, C);
  } else {
    char *str_a = "Total time is %.9f s\n";
    printf(str_a, time_elapsed(stop, start));
  }

  return 0;
}
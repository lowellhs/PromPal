#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file_reader.c"

int main(int argc, char **argv) {
  int i, j, k;
  struct timeval start, stop;

  int n, print_flag, from_file;
  if (argc == 5) {
    n = atoi(argv[2]);
    from_file = 0;
    print_flag = atoi(argv[4]);
  } else if (argc == 9) {
    n = atoi(argv[2]);
    from_file = 1;
    print_flag = atoi(argv[8]);
  } else {
    return 0;
  }

  double A[n][n];
  double B[n][n];
  double C[n][n];

  if (from_file) {
    char *file_matrix1 = argv[4];
    char *file_matrix2= argv[6];
    char *delim = " ";
  
    read_matrix(n, n, A, file_matrix1, delim);
    read_matrix(n, n, B, file_matrix2, delim);
  } else {
    for (i=0; i<n; i++) {
      for (j=0; j<n; j++) {
        A[i][j] = ((i + j) % 17) + 1;
        B[i][j] = ((i * j) % 19) + 1;
      }
    }
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
    printf(
      "Done in %.6f seconds.\n", 
      (stop.tv_sec+stop.tv_usec*1e-6)-(start.tv_sec+start.tv_usec*1e-6)
    );
  }

  return 0;
}
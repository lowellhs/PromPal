#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file_reader.c"

int main(int argc, char **argv) {
  int i, j;
  struct timeval start, stop;

  int m, n, print_flag, from_file;
  if (argc == 7) {
    m = atoi(argv[2]);
    n = atoi(argv[4]);
    from_file = 0;
    print_flag = atoi(argv[6]);
  } else if (argc == 11) {
    m = atoi(argv[2]);
    n = atoi(argv[4]);
    from_file = 1;
    print_flag = atoi(argv[10]);
  } else {
    return 0;
  }

  int ROWS_A = m;     // Ukuran baris matriks A (n x n)
  int COLS_A = n;     // Ukuran kolom matriks A (n x n)
  int VECSIZE_x = n;  // Ukuran vektor x (n x 1)
  int VECSIZE_b = m;  // Ukuran vektor b (m x 1)

  double A[ROWS_A][COLS_A];
  double x[VECSIZE_x];
  double b[VECSIZE_b];

  if (from_file) {
    char *file_matrix = argv[6];
    char *file_vector = argv[8];
    char *delim = " ";

    read_matrix(m, n, A, file_matrix, delim);
    read_vector(n, x, file_vector, delim);
  } else {
    for (i=0; i<ROWS_A; i++) {
      for (j=0; j<COLS_A; j++) {
        A[i][j] = ((i + j) % 17) + 1;
      }
    }
    for (i=0; i<VECSIZE_x; i++) {
      x[i] = i;
    }
  }

  gettimeofday(&start, 0);
  for (i=0; i<ROWS_A; i++) {
    b[i] = 0.0;
    for (j=0; j<COLS_A; j++) {
      b[i] = b[i] + A[i][j] * x[j];
    }
  }
  gettimeofday(&stop, 0);

  if (print_flag == 1) {
    print_vector(VECSIZE_b, b);
  } else {
    printf(
      "Done in %.6f seconds.\n", 
      (stop.tv_sec+stop.tv_usec*1e-6)-(start.tv_sec+start.tv_usec*1e-6)
    );
  }

  return 0;
}
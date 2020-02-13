#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "helper.c"

#define m 8
#define n 8

double A[m][n], x[n], b[m];

int main(int argc, char **argv) {
  int i, j, print_flag, from_file;
  struct timeval start, stop;


  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  if (from_file) {
    read_matrix(m, n, A, argv[3], " ");
    read_vector(n, x, argv[4], " ");
  } else {
    init_matrix(m, n, A);
    init_vector(n, x);
  }

  gettimeofday(&start, 0);
  for (i=0; i<m; i++) {
    b[i] = 0.0;
    for (j=0; j<n; j++) {
      b[i] = b[i] + A[i][j] * x[j];
    }
  }
  gettimeofday(&stop, 0);

  if (print_flag == 1) {
    print_vector(m, b);
  } else {
    char *str_a = "Total time is %.9f seconds.\n";
    printf(str_a, time_elapsed(stop, start));
  }

  return 0;
}
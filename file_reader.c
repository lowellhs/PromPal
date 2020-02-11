#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXCHAR 10000

int read_matrix(int m, int n, double A[m][n], char *filename, char *delim) {
  FILE *fp;
  char str[MAXCHAR];
  char *ptr;
  double value;
  int i,j;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    return 1;
  }

  i = 0;
  while (fgets(str, MAXCHAR, fp) != NULL) {
		ptr = strtok(str, delim);
    j = 0;
    while (ptr != NULL) {
      value = strtod(ptr, NULL);
      A[i][j] = value;
      ptr = strtok(NULL, delim);
      j++;
    }
    i++;
  }

  fclose(fp);
  return 0;
}

int read_vector(int n, double v[n], char *filename, char *delim) {
  FILE *fp;
  char str[MAXCHAR];
  int i;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    return 1;
  }

  i = 0;
  while (fgets(str, MAXCHAR, fp) != NULL) {
    v[i] = strtod(str, NULL);
    i++;
  }

  fclose(fp);
  return 0;
}

void print_matrix(int m, int n, double A[m][n]) {
  int i, j;
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
      printf("%f ", A[i][j]);
    }
    printf("%s\n", "");
  }
  printf("%s\n", "");
}

void print_vector(int n, double v[n]) {
  int i;
  for (i=0; i<n; i++) {
    printf("%f\n", v[i]);
  }
  printf("%s\n", "");
}

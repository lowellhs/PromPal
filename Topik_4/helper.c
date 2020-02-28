#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXCHAR 10000

int read_csv(int m, int n, int maxLen, char A[m][n][maxLen], char *filename) {
  FILE *fp;
  int c, currRow, currCol, currChar, i, j;
  fp = fopen(filename, "r");

  currRow = 0;
  currCol = 0;
  currChar = 0;
  while(1) {
    c = fgetc(fp);
    if (feof(fp)) {
      break;
    }
    if (c == ',') {
      A[currRow][currCol][currChar] = '\0';
      currChar = 0;
      currCol += 1;
    } else if (c == '\n') {
      A[currRow][currCol][currChar] = '\0';
      currChar = 0;
      currCol = 0;
      currRow += 1;
    } else {
      A[currRow][currCol][currChar] = c;
      currChar += 1;
    }
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
  while ((i < n) && (fgets(str, MAXCHAR, fp) != NULL)) {
    v[i] = strtod(str, NULL);
    i++;
  }

  fclose(fp);
  return 0;
}

int matrix_equals(int m, int n, double A[m][n], double B[m][n]) {
  int i,j,k;
  k = 1;
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
      k = k && ((A[i][j] - B[i][j]) <= 1e-12);
    }
  }
  return k;
}

int vector_equals(int m, double a[m], double b[m]) {
  int i,k;
  k = 1;
  for (i=0; i<m; i++) {
    k = k && ((a[i] - b[i]) <= 1e-12);
  }
  return k;
}

void init_matrix(int m, int n, double A[m][n]) {
  int i,j;
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
      A[i][j] = i + j;
    }
  }
}

void init_matrix_i(int n, double A[n][n]) {
  int i,j;
  for (i=0; i<n; i++) {
    for (j=0; j<n; j++) {
      if (i == j) {
        A[i][j] = 1.0;
      } else {
        A[i][j] = 0.0;
      }
    }
  }
}

void init_vector(int n, double v[n]) {
  int i;
  for (i=0; i<n; i++) {
    v[i] = i;
  }
}

void print_matrix(int m, int n, double A[m][n]) {
  int i, j;
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
      if (j == n-1) {
        printf("%f\n", A[i][j]);
      } else {
        printf("%f ", A[i][j]);
      }
    }
  }
}

void print_vector(int n, double v[n]) {
  int i;
  for (i=0; i<n; i++) {
    printf("%f\n", v[i]);
  }
}

double norm_vector(int n, double veca[n], double vecb[n]) {
  int i;
  double sum, diff;
  sum = 0.0;
  for (i=0; i<n; i++) {
    diff = vecb[i] - veca[i];
    sum = sum + diff*diff;
  }
  return sqrt(sum);
}
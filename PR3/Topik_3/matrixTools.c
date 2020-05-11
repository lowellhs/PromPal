#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXCHAR 524288

int read_matrix(int n, float *A, char *filename, char *delim) {
  FILE *fp;
  char str[MAXCHAR];
  char *ptr;
  float value;
  int i,j;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    return 1;
  }

  i = 0;
  while ((i < n) && (fgets(str, MAXCHAR, fp) != NULL)) {
		ptr = strtok(str, delim);
    j = 0;
    while (ptr != NULL) {
      value = strtod(ptr, NULL);
      A[i*n+j] = value;
      ptr = strtok(NULL, delim);
      j++;
    }
    i++;
  }

  fclose(fp);
  return 0;
}

int read_vector(int n, float *v, char *filename, char *delim) {
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

float norm_vector(int n, float *veca, float *vecb) {
  int i;
  float sum, diff;
  sum = 0.0;
  for (i=0; i<n; i++) {
    diff = vecb[i] - veca[i];
    sum = sum + diff*diff;
  }
  return sqrt(sum);
}


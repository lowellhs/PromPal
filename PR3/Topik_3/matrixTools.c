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
  float maxNorm = 0.0, diff;
  for (int i=0; i<n; i++) {
    diff = fabs(vecb[i]-veca[i]);
    if (diff > maxNorm) maxNorm = diff;
  }
  return maxNorm;
}

void printMatrix(int n, float *A)
{
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      printf("%10.3f ", A[n*i+j]);
    }
    printf("\n");
  }
}

void printVector(int n, float *x)
{
  for (int i=0; i<n; i++)
  {
    printf("%10.3f\n", x[i]);
  }
}

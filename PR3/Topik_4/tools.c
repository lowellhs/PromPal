#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXCHAR 10000

int read_csv(int m, int n, char **A, char *filename) {
  FILE *fp;
  int c, currRow, currCol, currChar;
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
      A[currRow*n + currCol][currChar] = '\0';
      currChar = 0;
      currCol += 1;
    } else if (c == '\n') {
      A[currRow*n + currCol][currChar] = '\0';
      currChar = 0;
      currCol = 0;
      currRow += 1;
    } else {
      A[currRow*n + currCol][currChar] = c;
      currChar += 1;
    }
  }

  fclose(fp);
  return 0;
}

char** mallocData(int m, int n, int maxLen)
{
  char** data = (char **)malloc(m * n * sizeof(char*));
  for (int i = 0; i < (m*n); i++) data[i] = (char *)malloc(maxLen * sizeof(char));
  return data;
}
void freeData(int m, int n, char **data)
{
  for (int i = 0; i < (m*n); i++) free(data[i]);
  free(data);
}

float* mallocUni(int m)
{
  float *y;
  cudaMallocManaged((void **)&y, m*sizeof(float));
  return y;
}
void freeUni(float *y)
{
  cudaFree(y);
}

int* mallocUni_int(int m)
{
  int *y;
  cudaMallocManaged((void **)&y, m*sizeof(int));
  return y;
}
void freeUni_int(int *y)
{
  cudaFree(y);
}

int* mallocY(int m)
{
  return (int *)malloc(m*sizeof(int));
}
void freeY(int *y)
{
  free(y);
}

void getXandY(int m, int n, char **data, float *X, int *y)
{
  for (int i=0; i<m; i++)
  {
    for (int j=0; j<(n-1); j++)
    {
      X[i*(n-1)+j] = strtof(data[i*n+j], NULL);
    }
    y[i] = atoi(data[i*n+(n-1)]);
  }
}

void print_matrix(int m, int n, float *X)
{
  for(int i=0; i<m; i++)
  {
    for (int j=0; j<n; j++)
    {
      printf("%.6f ", X[i*n+j]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_matrix_int(int m, int n, int *X)
{
  for(int i=0; i<m; i++)
  {
    for (int j=0; j<n; j++)
    {
      printf("%d ", X[i*n+j]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_vector(int m, float *y)
{
  for(int i=0; i<m; i++)
  {
    printf("%.6f\n", y[i]);
  }
  printf("\n");
}

void print_vector_int(int m, int *y)
{
  for(int i=0; i<m; i++)
  {
    printf("%d\n", y[i]);
  }
  printf("\n");
}

int major_num(int n, int maks, int *labels) {
  int i, indeks, maksValue, arr[maks];
  for (i=0; i<maks; i++) arr[i] = 0;
  for (i=0; i<n; i++) arr[labels[i]] += 1;
  maksValue = -1;
  for (i=0; i<maks; i++) {
    if (arr[i] > maksValue) {
      maksValue = arr[i];
      indeks = i;
    }
  }
  return indeks;
}

float accuracy(int n, int *y1, int *y2) {
  int equals = 0;
  for (int i=0; i<n; i++) {
    if (y1[i] == y2[i]) {
      equals += 1;
    }
  }
  return (equals*1.0/n * 100);
}

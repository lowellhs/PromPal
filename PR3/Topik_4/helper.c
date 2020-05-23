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

float** mallocUniX(int m, int n)
{
  float **X;
  cudaMallocManaged((void **)&X, m*sizeof(float *));
  for (int i=0; i<m; i++) cudaMallocManaged((void **)&(X[i]), n*sizeof(float));
  return X;
}
void freeUniX(int m, float **X)
{
  for (int i = 0; i < m; i++) cudaFree(X[i]);
  cudaFree(X);
}

float* mallocUniY(int m)
{
  float *y;
  cudaMallocManaged((void **)&y, m*sizeof(float));
  return y;
}
void freeUniY(float *y)
{
  cudaFree(y);
}

void getXandY(int m, int n, char **data, float *X, float *y)
{
  for (int i=0; i<m; i++)
  {
    for (int j=0; j<(n-1); j++)
    {
      X[i*(n-1)+j] = strtof(data[i*n+j], NULL);
    }
    y[i] = strtof(data[i*n+(n-1)], NULL);
  }
}

/*
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

void init_vector(int n, int v[n]) {
  int i;
  for (i=0; i<n; i++) {
    v[i] = i;
  }
}
void init_vector_part(int n, int v[n], int start) {
  int i;
  for (i=0; i<n; i++) {
    v[i] = start+i;
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
void print_vector_int(int n, int v[n]) {
  int i;
  for (i=0; i<n; i++) {
    printf("%d\n", v[i]);
  }
}

double accuracy(int n, int veca[n], int vecb[n]) {
  int i, equals;
  equals = 0;
  for (i=0; i<n; i++) {
    if (veca[i] == vecb[i]) {
      equals += 1;
    }
  }
  return (equals*1.0/n * 100);
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
double norm_vector_int(int n, int veca[n], int vecb[n]) {
  int i;
  double sum, diff;
  sum = 0.0;
  for (i=0; i<n; i++) {
    diff = vecb[i] - veca[i];
    sum = sum + diff*diff;
  }
  return sqrt(sum);
}

int major_num(int n, int maks, int labels[n]) {
  int i, indeks, maksValue, arr[maks];
  for (i=0; i<maks; i++) {
    arr[i] = 0;
  }
  for (i=0; i<n; i++) {
    arr[labels[i]] += 1;
  }
  maksValue = -1;
  for (i=0; i<maks; i++) {
    if (arr[i] > maksValue) {
      maksValue = arr[i];
      indeks = i;
    }
  }
  return indeks;
}
*/

void merge(double *arr, int *indices, int l, int m, int r) {
  int i,j,k;
  int n1 = m - l + 1;
  int n2 = r - m;

  double L[n1], R[n2];
  int Li[n1], Ri[n2];

  for (i=0; i<n1; i++) {
    L[i] = arr[l+i];
    Li[i] = indices[l+i];
  }
  for (j=0; j<n2; j++) {
    R[j] = arr[m+1+j];
    Ri[j] = indices[m+1+j];
  }

  i = 0;
  j = 0;
  k = l;
  while (i<n1 && j<n2) {
    if (L[i] <= R[j]) {
      arr[k] = L[i];
      indices[k] = Li[i];
      i++;
    } else {
      arr[k] = R[j];
      indices[k] = Ri[j];
      j++;
    }
    k++;
  }

  while (i<n1) {
    arr[k] = L[i];
    indices[k] = Li[i];
    i++;
    k++;
  }

  while (j<n2) {
    arr[k] = R[j];
    indices[k] = Ri[j];
    j++;
    k++;
  }
}

void mergeSort(double *arr, int *indices, int l, int r) {
  if (l < r) {
    int m = l+(r-l)/2;
    mergeSort(arr, indices, l, m);
    mergeSort(arr, indices, m+1, r);
    merge(arr, indices, l, m, r);
  }
}

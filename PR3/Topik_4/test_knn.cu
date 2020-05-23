#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cuda.h>
#include "cublas_v2.h"
#include "helper.c"

#define MAX_LEN 1000

__global__ void calculateDistances(cublasHandle_t handle, int m, int n, float **X_train, float *row_test, float *distances)
{
  int id = blockIdx.x*gridDim.x + threadIdx.x;
  float al = -1.0;
  float dist;
  cublasSaxpy(handle, n, &al, X_train[id], 1, row_test, 1);
  cublasSnrm2(handle, n, row_test, 1, &dist);
  distances[id] = dist;
}

void getTopKRows(int m, int k, float *distances, int *indices)
{
  cublasHandle_t handle;
  cublasCreate(&handle);

  int idxMin, idxMax;
  float maxVal;
  cublasIsamax(handle, m, distances, 1, &idxMax);
  cudaDeviceSynchronize();
  maxVal = distances[idxMax-1];
  for (int i=0; i<k; i++)
  {
    cublasIsamin(handle, m, distances, 1, &idxMin);
    cudaDeviceSynchronize();
    indices[i] = idxMin-1;
    distances[idxMin-1] = maxVal;
  }

  cublasDestroy(handle);
}

int main(int argc, char **argv)
{
  int m = 150;
  int t = 3;
  int n = 5;
  
  float **X_train = mallocUniX(m, n-1), **X_test = mallocUniX(t, n-1);
  float *y_train = mallocUniY(m), *y_test = mallocUniY(t), *distances = mallocUniY(m);

  char **data_train = mallocData(m, n, MAX_LEN);
  char **data_test = mallocData(t, n, MAX_LEN);
  read_csv(m, n, data_train, "test_input/Iris_150.csv");
  read_csv(t, n, data_test, "test_input/Iris_test.csv");
  getXandY(m, n, data_train, X_train, y_train);
  getXandY(t, n, data_test, X_test, y_test);
  freeData(m, n, data_train);
  freeData(t, n, data_test);

  cublasHandle_t handle;
  cublasCreate(&handle);
  calculateDistances<<<1,150>>>(handle, m, n, X_train, X_test[0], distances);
  cudaDeviceSynchronize();
  cublasDestroy(handle);  

  freeUniX(m, X_train); freeUniX(t, X_test);
  freeUniY(y_train); freeUniY(y_test);

  /*
  int m = atoi(argv[1]);
  int n = atoi(argv[2]);
  int k = atoi(argv[3]);

  float *X_train, *X_test, *distances;
  cudaMallocManaged((void **)&X_train, m*n*sizeof(float));
  cudaMallocManaged((void **)&X_test, m*n*sizeof(float));
  cudaMallocManaged((void **)&X_test, m*sizeof(float));
  for (int i=0; i<m; i++)
  {
    for (int j=0; j<n; j++)
    {
      X_train[i*n+j] = i*n+j;
      X_test[i*n+j] = i*n+j+1;
    }
  }

  cudaFree(X_train); cudaFree(X_test);
  */
  return 0;
}

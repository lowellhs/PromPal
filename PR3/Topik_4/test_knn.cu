#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cuda.h>
#include <math.h>
#include "cublas_v2.h"
#include "helper.c"

#define MAX_LEN 1000

__global__ void getSquaredNorm(int m, int n, float *X, float *normX)
{
  int idx = blockIdx.x*gridDim.x + threadIdx.x;
  if (idx < m)
  {
    float norm = 0.0;
    for (int j=0; j<n; j++)
    {
      float val = X[idx*n+j];
      norm += (val*val);
    }
    normX[idx] = norm;
  }
}

__global__ void calculateDistance(int m, int t, float *normX, float *normY, float *xTy)
{
  int idx = blockIdx.x*gridDim.x + threadIdx.x;
  if (idx < m*t)
  {
    int i = idx/t, j = idx%t;
    float squaredDist = normX[i] + normY[j] + xTy[idx];
    float dist = sqrt(squaredDist);
    xTy[idx] = dist;
  }
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

void matmulTrainTest(int m, int n, int k, float *A, float *B, float *C)
{
  cublasHandle_t handle;
  cublasCreate(&handle);
  float al = -2.0f, bet = 0.0f;
  cublasSgemm(handle, CUBLAS_OP_T, CUBLAS_OP_N, n, m, k, &al, B, k, A, k, &bet, C, n);
  cublasDestroy(handle); 
}

int main(int argc, char **argv)
{
  int m = 150;
  int t = 3;
  int n = 5;
  
  float *X_train = mallocUni(m*(n-1)), *X_test = mallocUni(t*(n-1));
  float *y_train = mallocUni(m), *y_test = mallocUni(t);
  float *distances = mallocUni(m);
  float *normX_train = mallocUni(m), *normX_test = mallocUni(t);
  float *trainTtest = mallocUni(m*t);

  char **data_train = mallocData(m, n, MAX_LEN);
  char **data_test = mallocData(t, n, MAX_LEN);
  read_csv(m, n, data_train, "test_input/Iris_150.csv");
  read_csv(t, n, data_test, "test_input/Iris_test.csv");
  getXandY(m, n, data_train, X_train, y_train);
  getXandY(t, n, data_test, X_test, y_test);
  freeData(m, n, data_train);
  freeData(t, n, data_test);

  getSquaredNorm<<<(int)ceil(m/1024.0), 1024>>>(m, n-1, X_train, normX_train);
  getSquaredNorm<<<(int)ceil(m/1024.0), 1024>>>(t, n-1, X_test, normX_test);
  matmulTrainTest(m, t, n-1, X_train, X_test, trainTtest);
  calculateDistance<<<(int)ceil(m/1024.0), 1024>>>(m, t, normX_train, normX_test, trainTtest);
  cudaDeviceSynchronize();
  print_matrix(m, t, trainTtest);

  print_vector(t, normX_test);
  freeUni(X_train); freeUni(X_test);
  freeUni(y_train); freeUni(y_test);

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

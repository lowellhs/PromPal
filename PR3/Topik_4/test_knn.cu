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
    int i = idx/m, j = idx%m;
    float squaredDist = normX[j] + normY[i] + xTy[idx];
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
  int m = 60000;
  int t = 10000;
  int n = 33;
  int k = 5;
  int labels = 10;
  
  float *X_train = mallocUni(m*(n-1)), *X_test = mallocUni(t*(n-1));
  int   *y_train = mallocY(m), *y_test = mallocY(t), *y_pred = mallocY(t);
  float *normX_train = mallocUni(m), *normX_test = mallocUni(t);
  float *trainTtest = mallocUni(t*m);

  char **data_train = mallocData(m, n, MAX_LEN);
  char **data_test = mallocData(t, n, MAX_LEN);
  read_csv(m, n, data_train, "test_input/MNIST_train_60k.csv");
  read_csv(t, n, data_test, "test_input/MNIST_test_10k.csv");
  getXandY(m, n, data_train, X_train, y_train);
  getXandY(t, n, data_test, X_test, y_test);
  freeData(m, n, data_train);
  freeData(t, n, data_test);

  printf("Start predicting...\n");
  cudaEvent_t start, stop; cudaEventCreate(&start); cudaEventCreate(&stop); cudaEventRecord(start);
  getSquaredNorm<<<(int)ceil(m/1024.0), 1024>>>(m, n-1, X_train, normX_train);
  getSquaredNorm<<<(int)ceil(m/1024.0), 1024>>>(t, n-1, X_test, normX_test);
  matmulTrainTest(t, m, n-1, X_test, X_train, trainTtest);
  calculateDistance<<<(int)ceil(m/1024.0), 1024>>>(m, t, normX_train, normX_test, trainTtest);
  cudaEventRecord(stop); cudaEventSynchronize(stop); cudaDeviceSynchronize();
  float milliseconds = 0; cudaEventElapsedTime(&milliseconds, start, stop);
  cudaDeviceSynchronize();
  for (int i=0; i<t; i++)
  {
    int *indices = (int *)malloc(k*sizeof(int));
    int *pred_labels = (int *)malloc(k*sizeof(int));
    getTopKRows(m, k, &(trainTtest[i*m]), indices);
    for (int j=0; j<k; j++) pred_labels[j] = (int)y_train[indices[j]];
    y_pred[i] = major_num(k, labels, pred_labels);
    free(indices); free(pred_labels);
  }
  // print_vector_int(t, y_pred);
  // cudaEventRecord(stop); cudaEventSynchronize(stop); cudaDeviceSynchronize();
  // float milliseconds = 0; cudaEventElapsedTime(&milliseconds, start, stop);
  printf("%.6f\n", milliseconds*1e-3);

  freeUni(X_train); freeUni(X_test);
  freeY(y_train); freeY(y_test); freeY(y_pred);
  freeUni(normX_train); freeUni(normX_test);
  freeUni(trainTtest);

  return 0;
}

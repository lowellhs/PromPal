#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cuda.h>
#include <math.h>
#include "cublas_v2.h"
#include "tools.c"

#define MAX_LEN 1000
#define BLOCK_SIZE 512

__global__ void getSquaredNorm(int m, int n, float *X, float *normX)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
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
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  if (idx < m*t)
  {
    int i = idx/m, j = idx%m;
    float squaredDist = normX[j] + normY[i] + xTy[idx];
    float dist = sqrt(squaredDist);
    xTy[idx] = dist;
  }
}

__global__ void parallel_sort(int t, int m, int k, float *matrix, int *result)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  if (idx < t)
  {
    for (int i=0; i<k; i++)
    {
      int min_idx = -1, min_idx_g = -1;
      float min_val;
      for (int j=0; j<m; j++)
      {
        int valid = 1;
        for (int jj=0; jj<i; jj++) valid = valid && (j != result[idx*k+jj]);
        if (valid)
        {
          float val = matrix[idx*m+j];
          if ((min_idx == -1) || (val < min_val))
          {
            min_val = val;
            min_idx = idx*m+j;
            min_idx_g = j;
          }
        }
      }
      result[idx*k+i] = min_idx_g;
    }
  }
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
  int m = atoi(argv[1]);
  int t = atoi(argv[2]);
  int n = atoi(argv[3]);
  int k = atoi(argv[4]);
  int labels = atoi(argv[5]);
  
  float *X_train = mallocUni(m*(n-1)), *X_test = mallocUni(t*(n-1));
  int   *y_train = mallocY(m), *y_test = mallocY(t), *y_pred = mallocY(t), *result = mallocUni_int(t*k);
  float *normX_train = mallocUni(m), *normX_test = mallocUni(t);
  float *trainTtest = mallocUni(t*m);

  char **data_train = mallocData(m, n, MAX_LEN);
  char **data_test = mallocData(t, n, MAX_LEN);
  read_csv(m, n, data_train, argv[6]);
  read_csv(t, n, data_test, argv[7]);
  getXandY(m, n, data_train, X_train, y_train);
  getXandY(t, n, data_test, X_test, y_test);
  freeData(m, n, data_train);
  freeData(t, n, data_test);

  printf("Start predicting...\n");
  cudaEvent_t start, stop; cudaEventCreate(&start); cudaEventCreate(&stop); cudaEventRecord(start);
  getSquaredNorm<<<(int)ceil((m*1.0)/BLOCK_SIZE), BLOCK_SIZE>>>(m, n-1, X_train, normX_train);
  getSquaredNorm<<<(int)ceil((m*1.0)/BLOCK_SIZE), BLOCK_SIZE>>>(t, n-1, X_test, normX_test);
  matmulTrainTest(t, m, n-1, X_test, X_train, trainTtest);
  calculateDistance<<<(int)ceil((m*t*1.0)/BLOCK_SIZE), BLOCK_SIZE>>>(m, t, normX_train, normX_test, trainTtest);
  parallel_sort<<<(int)ceil((t*1.0)/BLOCK_SIZE), BLOCK_SIZE>>>(t, m, k, trainTtest, result);
  cudaDeviceSynchronize();
  for (int i=0; i<t; i++)
  {
    int *pred_labels = (int *)malloc(k*sizeof(int));
    for (int j=0; j<k; j++) pred_labels[j] = (int)y_train[result[i*k+j]];
    y_pred[i] = major_num(k, labels, pred_labels);
    free(pred_labels);
  }
  cudaEventRecord(stop); cudaEventSynchronize(stop); cudaDeviceSynchronize();
  float milliseconds = 0; cudaEventElapsedTime(&milliseconds, start, stop);
  printf("Done in %.6f s\n", milliseconds*1e-3);
  float acc = accuracy(t, y_test, y_pred);
  printf("Accuracy: %.6f %%\n", acc);

  freeUni(X_train); freeUni(X_test);
  freeY(y_train); freeY(y_test); freeY(y_pred); freeUni_int(result);
  freeUni(normX_train); freeUni(normX_test);
  freeUni(trainTtest);

  return 0;
}

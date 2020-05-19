#include <stdio.h>
#include <stdlib.h>
#include "matrixTools.c"
#include <sys/time.h>
#include <cuda.h>
#include "cublas_v2.h"

#define TOL 1e-6
#define limit_iter 10000
#define BLOCK_SIZE 50

__device__ int flag;

__global__ void getDLU(int n, float *A, float *D_inv, float *LU)
{
  int i = blockIdx.y*blockDim.y + threadIdx.y;
  int j = blockIdx.x*blockDim.x + threadIdx.x;
  if (i<n && j<n)
  {
    if (i == j)
    {
      D_inv[i*n+j] = 1.0/(A[i*n+j]);
    }
    else
    {
      LU[i*n+j] = -A[i*n+j];
    }
  }
}

__global__ void checkConvergence(int n, float *x_iter, float *x_iter_new)
{
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i<n)
  {
    if (fabs(x_iter[i]-x_iter_new[i]) > TOL) flag = 0;
    x_iter[i] = x_iter_new[i];
  }
}

int main(int argc, char **argv)
{
  int sizeCounter = atoi(argv[1]);
  int tests = atoi(argv[2]);
  for (int counter=3; counter < 3+sizeCounter; counter++)
  {
    for (int testCounter=0; testCounter < tests; testCounter++)
    {
      cudaEvent_t start, stop;

      float *A, *b, *x, *x_iter, *x_iter_new, *D_inv, *LU, *T, *c;
      float al, bet;

      int n = atoi(argv[counter]);
      cudaMallocManaged(&A, n*n*sizeof(float));
      cudaMallocManaged(&b, n*sizeof(float));
      cudaMallocManaged(&x, n*sizeof(float));
      cudaMallocManaged(&x_iter, n*sizeof(float));
      cudaMallocManaged(&x_iter_new, n*sizeof(float));
      cudaMallocManaged(&D_inv, n*n*sizeof(float));
      cudaMallocManaged(&LU, n*n*sizeof(float));
      cudaMallocManaged(&T, n*n*sizeof(float));
      cudaMallocManaged(&c, n*sizeof(float));
      
      char file_A[80], file_b[80], file_x[80];
      sprintf(file_A, "test_input/matrix_A_%dx%d.txt", n, n);
      sprintf(file_b, "test_input/vector_b_%dx1.txt", n);
      sprintf(file_x, "test_input/vector_x_%dx1.txt", n);
      read_matrix(n, A, file_A, " ");
      read_vector(n, b, file_b, " ");
      read_vector(n, x, file_x, " ");
      for (int i=0; i<n; i++) x_iter[i] = 0.0;
    
      cudaEventCreate(&start);
      cudaEventCreate(&stop);
      cudaEventRecord(start);

      int blockDim = 32;
      int gridDim = (int)ceil(n/32.0);
      dim3 aa = dim3(gridDim,gridDim);
      dim3 bb = dim3(blockDim,blockDim);
      getDLU<<<aa,bb>>>(n, A, D_inv, LU);
      cudaDeviceSynchronize();

      int gridSize = (int)ceil(1.0*n/BLOCK_SIZE);
      cublasHandle_t handle;
      cublasCreate(&handle);
      al = 1.0f, bet = 0.0f;
      cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, n, n, n, &al, LU, n, D_inv, n, &bet, T, n);
      cublasSgemv(handle, CUBLAS_OP_N, n, n, &al, D_inv, n, b, 1, &bet, c, 1);
      cudaDeviceSynchronize();
      int k = 0, isConverged;
      do {
        isConverged = 1;

        cudaMemcpyToSymbol(flag, &isConverged, sizeof(int));
        cublasSgemv(handle, CUBLAS_OP_T, n, n, &al, T, n, x_iter, 1, &bet, x_iter_new, 1);
        cublasSaxpy(handle, n, &al, c, 1, x_iter_new, 1);
        checkConvergence<<<gridSize, BLOCK_SIZE>>>(n, x_iter, x_iter_new);
        cublasScopy(handle, n, x_iter_new, 1, x_iter, 1);
        cudaMemcpyFromSymbol(&isConverged, flag, sizeof(int));
        cudaDeviceSynchronize();

        k++;
      } while (k < limit_iter && !isConverged);
      cublasDestroy(handle);

      cudaEventRecord(stop);
      cudaEventSynchronize(stop);
      float milliseconds = 0; cudaEventElapsedTime(&milliseconds, start, stop);
      cudaDeviceSynchronize();

      // cudaError_t code=cudaGetLastError();
      // printf("%s\n", cudaGetErrorString(code));
      printf("%d ", n);
      printf("%.6f ", milliseconds*1e-3);
      printf("%.9f ", norm_vector(n, x_iter, x));
      printf("%d\n", k);

      cudaFree(A); cudaFree(b); cudaFree(x); cudaFree(x_iter); cudaFree(x_iter_new); cudaFree(D_inv); cudaFree(LU);
      cudaFree(T); cudaFree(c);
    }
  }
  return 0;
}

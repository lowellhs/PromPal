#include "matrixTools.c"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>
#include <cstdlib>
#include "cublas_v2.h"

int main(int argc, char **argv)
{
  int sizeCounter = atoi(argv[1]);
  int tests = atoi(argv[2]);
  for (int counter=3; counter < 3+sizeCounter; counter++)
  {
    for (int testCounter=0; testCounter < tests; testCounter++)
    {
    cudaEvent_t start, stop;

    float *A_h, *B_h, *C_h, *C2_h;  // pointers to host memory
    float *A_d, *B_d, *C_d;         // pointers to device memory

    // size of matrix (n*n)
    int n = atoi(argv[counter]);
    size_t size = n*n*sizeof(float);

    // allocate array on host
    A_h  = (float *)malloc(size);
    B_h  = (float *)malloc(size);
    C_h  = (float *)malloc(size);
    C2_h = (float *)malloc(size);

    // allocate array on device
    cudaMalloc((void **) &A_d, size);
    cudaMalloc((void **) &B_d, size);
    cudaMalloc((void **) &C_d, size);

    // initializtion of host data
    initIdentityMatrix(n, A_h);
    initRandomMatrix(n, B_h);
    
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    // copy data from host to device
    cudaMemcpy(A_d, A_h, n*n*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(B_d, B_h, n*n*sizeof(float), cudaMemcpyHostToDevice);

    // do calculation on device
    int lda=n, ldb=n, ldc=n;
    const float alf = 1;
    const float bet = 0;
    const float *alpha = &alf;
    const float *beta = &bet;
    cublasHandle_t handle;
    cublasCreate(&handle);
    cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, n, n, n, alpha, B_d, lda, A_d, ldb, beta, C_d, ldc);
    cublasDestroy(handle);
    cudaMemcpy(C2_h, C_d, n*n*sizeof(float), cudaMemcpyDeviceToHost);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float milliseconds = 0; cudaEventElapsedTime(&milliseconds, start, stop);
    cudaDeviceSynchronize();

    float err = errorMatrix(n, C2_h, B_h);
    printf("%d ", n);
    printf("%.6f ", milliseconds*1e-3);
    printf("%.6f\n", err);

    // Cleanup
    free(A_h); free(B_h); free(C_h); free(C2_h);
    cudaFree(A_d); cudaFree(B_d); cudaFree(C_d);
    }
  }
}

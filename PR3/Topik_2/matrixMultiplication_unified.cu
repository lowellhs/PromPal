#include "matrixTools.c"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>
#include <sys/time.h>

__global__ void matmulOnDevice(int n, float *A, float *B, float *C)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  int idy = blockIdx.y*blockDim.y + threadIdx.y;
  if (idx < n && idy < n)
  {
    float temp = 0.0;
    for (int i=0; i<n; i++)
    {
      temp += A[n*idy + i]*B[n*i + idx];
    }
    C[n*idy + idx] = temp;
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

    float *A, *B, *C;

    // size of matrix (n*n)
    int n = atoi(argv[counter]);
    size_t size = n*n*sizeof(float);

  	cudaMallocManaged(&A,size);
  	cudaMallocManaged(&B,size);
  	cudaMallocManaged(&C,size);

    // initializtion of host data
    initIdentityMatrix(n, A);
    initRandomMatrix(n, B);
    
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    // do calculation on device
    // Part 1 of 2. Compute execution configuration
    dim3 gridDim, blockDim;
    if (argc == (3+sizeCounter+4))
    {
      gridDim = dim3(atoi(argv[argc-4]), atoi(argv[argc-3]));
      blockDim = dim3(atoi(argv[argc-2]), atoi(argv[argc-1]));
    }
    else
    {
      if (n*n <= 1024)
      {
        gridDim = dim3(1, 1);
        blockDim = dim3(n, n);
      }
      else
      {
        gridDim = dim3((int)ceil(n/32.0), (int)ceil(n/32.0));
        blockDim = dim3(32, 32);
      }
    }
    
    // Part 2 of 2. Call matmulOnDevice kernel
    matmulOnDevice<<<gridDim, blockDim>>>(n, A, B, C);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float milliseconds = 0; cudaEventElapsedTime(&milliseconds, start, stop);
    cudaDeviceSynchronize();

    float err = errorMatrix(n, C, B);
    printf("%d (%d,%d) (%d,%d) ", n, gridDim.x, gridDim.y, blockDim.x, blockDim.y);
    printf("%.6f ", milliseconds*1e-3);
    printf("%.6f\n", err);

    // Cleanup
    cudaFree(A); cudaFree(B); cudaFree(C);
    }
  }
}


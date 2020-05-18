#include "matrixTools.c"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>
#include <sys/time.h>

#define TILE_WIDTH 32

__global__ void matmulOnDevice(int n, float *A, float *B, float *C)
{
  __shared__ float  a[TILE_WIDTH][TILE_WIDTH], b[TILE_WIDTH][TILE_WIDTH];
  int col =  blockIdx.x*blockDim.x + threadIdx.x;
  int row =  blockIdx.y*blockDim.y + threadIdx.y;

  float c = 0.0f;
  for (int k=0; k < (TILE_WIDTH+n-1)/TILE_WIDTH; k++)
  {
    if (k*TILE_WIDTH + threadIdx.x < n && row < n)
      a[threadIdx.y][threadIdx.x] = A[row*n+k*TILE_WIDTH+threadIdx.x];
    else
      a[threadIdx.y][threadIdx.x] = 0.0;

    if (k*TILE_WIDTH + threadIdx.y < n && col < n)
      b[threadIdx.y][threadIdx.x] = B[col+n*(k*TILE_WIDTH+threadIdx.y)];
    else
      b[threadIdx.y][threadIdx.x] = 0.0;
    __syncthreads();

    for (int kk=0; kk<TILE_WIDTH; ++kk)
      c += a[threadIdx.y][kk]*b[kk][threadIdx.x]; 
    __syncthreads();
  }

  if (row < n && col < n)
  {
    C[row*n+col] = c;
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
    matmulOnDevice<<<gridDim, blockDim>>>(n, A_d, B_d, C_d);
    cudaMemcpy(C2_h, C_d, n*n*sizeof(float), cudaMemcpyDeviceToHost);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float milliseconds = 0; cudaEventElapsedTime(&milliseconds, start, stop);
    cudaDeviceSynchronize();

    float err = errorMatrix(n, C2_h, B_h);
    printf("%d %.6f %.6f (%d,%d) (%d,%d)\n", n, milliseconds*1e-3, err, gridDim.x, gridDim.y, blockDim.x, blockDim.y);

    // Cleanup
    free(A_h); free(B_h); free(C_h); free(C2_h);
    cudaFree(A_d); cudaFree(B_d); cudaFree(C_d);
    }
  }
}


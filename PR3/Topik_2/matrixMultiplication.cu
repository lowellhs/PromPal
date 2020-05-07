#include "matrixTools.c"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>
#include <sys/time.h>

void randomMatrixGenerator(int n, float *A)
{
  srand(time(0));
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++) 
    {
      A[n*i+j] = rand();
    }
  }
}

void initIdentityMatrix(int n, float *I)
{
  for(int i=0; i<n; i++)
  {
    for(int j=0; j<n; j++)
    {
      if(i==j) I[n*i+j] = 1.0;
      else I[n*i+j] = 0.0;
    }
  }
  
}

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

void matmul(int n, float *A, float *B, float *C)
{
  for (int k=0; k<n; k++)
  {
    for (int j=0; j<n; j++)
    {
      C[n*k+j] = 0;
      for (int i=0; i<n; i++)
      {
        C[n*k+j] = C[n*k+j] + A[n*k+i] * B[n*i+j];
      }
    }
  }
}

void printMatrix(int n, float *A)
{
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      printf("%10.3f ", A[n*i+j]);
    }
    printf("\n");
  }
}

float errorMatrix(int n, float *A, float *B)
{
  float res = 0.0;
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      res += (A[n*i+j] - B[n*i+j]);
    }
  }
  return res;
}

int main(int argc, char **argv)
{
  struct timeval startCPU, stopCPU, startGPU, stopGPU;

  float *A_h, *B_h, *C_h, *C2_h;  // pointers to host memory
  float *A_d, *B_d, *C_d;         // pointers to device memory

  // size of matrix (n*n)
  int n = atoi(argv[1]);
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
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      A_h[n*i+j] = 0.0;
      B_h[n*i+j] = n*i+j;
      if (i==j) A_h[n*i+j] = 1.0;
    }
  }
  
  gettimeofday(&startGPU, 0);
  // copy data from host to device
  cudaMemcpy(A_d, A_h, n*n*sizeof(float), cudaMemcpyHostToDevice);
  cudaMemcpy(B_d, B_h, n*n*sizeof(float), cudaMemcpyHostToDevice);

  // do calculation on device
  // Part 1 of 2. Compute execution configuration
  int gridDimX = atoi(argv[2]);
  int gridDimY = atoi(argv[3]);
  int blockDimX = atoi(argv[4]);
  int blockDimY = atoi(argv[5]);
  dim3 gridSize = dim3(gridDimX, gridDimY);
  dim3 blockSize = dim3(blockDimX, blockDimY);
  printf("blockDim: (%d,%d), gridDim: (%d,%d)\n", blockDimX, blockDimY, gridDimX, gridDimY);

  // Part 2 of 2. Call matmulOnDevice kernel
  matmulOnDevice<<<gridSize, blockSize>>>(n, A_d, B_d, C_d);
  cudaMemcpy(C2_h, C_d, n*n*sizeof(float), cudaMemcpyDeviceToHost);

  cudaDeviceSynchronize();
  gettimeofday(&stopGPU, 0);

  // print matrix OR check matrix OR print time
  if (argc == 7 && atoi(argv[6]) == 0) printMatrix(n, C2_h);
  if (argc == 7 && atoi(argv[6]) == 1) // compare to CPU time and CPU result
  {
    // do calculation on host
    gettimeofday(&startCPU, 0);
    matmul(n, A_h, B_h, C_h);
    float err = errorMatrix(n, C2_h, C_h);
    gettimeofday(&stopCPU, 0);

    printf("CPU time : %.6f\n", (stopCPU.tv_sec+stopCPU.tv_usec*1e-6)-(startCPU.tv_sec+startCPU.tv_usec*1e-6));
    printf("error    : %.6f\n", err);
  }
  if (argc == 7 && atoi(argv[6]) == 2) // if I.B = C means B should equals C (A is identity matrix)
  {
    float err = errorMatrix(n, C2_h, B_h);
    printf("error    : %.6f\n", err);
  }
  printf("GPU time : %.6f\n", (stopGPU.tv_sec+stopGPU.tv_usec*1e-6)-(startGPU.tv_sec+startGPU.tv_usec*1e-6));

  // Cleanup
  free(A_h);
  free(B_h);
  free(C_h);
  free(C2_h);
  cudaFree(A_d);
  cudaFree(B_d);
  cudaFree(C_d);

}


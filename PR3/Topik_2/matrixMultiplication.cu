#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>

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

void checkMatrix(int n, float *A, float *B)
{
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      assert(A[n*i+j] == B[n*i+j]);
    }
  }
}

int main(int argc, char **argv)
{
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

  // print matrix OR check matrix OR print time
  cudaDeviceSynchronize();
  if (argc == 7 && atoi(argv[6]) == 0) printMatrix(n, C2_h);
  if (argc == 7 && atoi(argv[6]) == 1)
  {
    // do calculation on host
    matmul(n, A_h, B_h, C_h);
    checkMatrix(n, C2_h, C_h);
  }
  if (argc == 7 && atoi(argv[6]) == 2) printf("time");

  // Cleanup
  free(A_h);
  free(B_h);
  free(C_h);
  free(C2_h);
  cudaFree(A_d);
  cudaFree(B_d);
  cudaFree(C_d);

}

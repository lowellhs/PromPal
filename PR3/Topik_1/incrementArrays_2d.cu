// incrementArray.cu

#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>

void incrementArrayOnHost(float *a, int N)
{
  int i;
  for (i=0; i < N; i++) a[i] = a[i]+1.f;
}

__global__ void incrementArrayOnDevice(float *a, int N)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  int idy = blockIdx.y*blockDim.y + threadIdx.y;
  int id  = idy*gridDim.x*blockDim.x + idx;
  //printf("block: (%d,%d), thread:(%d), id: %d, val: %.6f\n", blockIdx.x, blockIdx.y, idx, id, a[id]);
  if (id<N) a[id] = a[id]+1.f;
}

int main(int argc, char **argv)
{
  float *a_h, *b_h; // pointers to host memory
  float *a_d; // pointer to device memory
  int i, N = atoi(argv[1]);
  size_t size = N*sizeof(float);

  // allocate arrays on host
  a_h = (float *)malloc(size);
  b_h = (float *)malloc(size);

  // allocate array on device
  cudaMalloc((void **) &a_d, size);
  // initialization of host data
  for (i=0; i<N; i++) a_h[i] = (float)i;
  // copy data from host to device
  cudaMemcpy(a_d, a_h, sizeof(float)*N, cudaMemcpyHostToDevice);
  
  // do calculation on host
  incrementArrayOnHost(a_h, N);

  // do calculation on device:
  // Part 1 of 2. Compute execution configuration
  int blockDimX = atoi(argv[2]);
  int blockDimY = atoi(argv[3]);
  int gridDimX = atoi(argv[4]);
  int gridDimY = atoi(argv[5]);

  dim3 blockSize = dim3(blockDimX, blockDimY);
  dim3 gridSize = dim3(gridDimX, gridDimY);
  printf("blockDim: (%d,%d), gridDim: (%d,%d)\n", blockDimX, blockDimY, gridDimX, gridDimY);

  // Part 2 of 2. Call incrementArrayOnDevice kernel
  incrementArrayOnDevice <<< gridSize, blockSize >>> (a_d, N);
  // Retrieve result from device and store in b_h
  cudaMemcpy(b_h, a_d, sizeof(float)*N, cudaMemcpyDeviceToHost);

  cudaDeviceSynchronize();

  // check results
  for (i=0; i<N; i++) assert(a_h[i] == b_h[i]);

  // cleanup
  free(a_h); free(b_h); cudaFree(a_d);
}

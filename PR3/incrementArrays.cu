// incrementArray.cu

#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <sys/time.h>

struct timeval t1, t2;

void incrementArrayOnHost(float *a, int N)
{
  int i;
  for (i=0; i < N; i++) a[i] = a[i]+1.f;
}

__global__ void incrementArrayOnDevice(float *a, int N)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  if (idx<N) a[idx] = a[idx]+1.f;
}

int main(void)
{
  float *a_h, *b_h; // pointers to host memory
  float *a_d; // pointer to device memory
  int i, N = 1e+9;
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
  
  
  gettimeofday(&t1, 0);
  // do calculation on host
  incrementArrayOnHost(a_h, N);
  gettimeofday(&t2, 0);
  double time1 = (1000000.0*(t2.tv_sec-t1.tv_sec) + t2.tv_usec-t1.tv_usec)/1000.0;
  printf("Time to generate:  %3.1f ms \n", time1);

  gettimeofday(&t1, 0);
  // do calculation on device:
  // Part 1 of 2. Compute execution configuration
  int blockSize = 8;
  int nBlocks = N/blockSize + (N%blockSize == 0?0:1);
  // Part 2 of 2. Call incrementArrayOnDevice kernel
  incrementArrayOnDevice <<< nBlocks, blockSize >>> (a_d, N);
  // Retrieve result from device and store in b_h
  cudaMemcpy(b_h, a_d, sizeof(float)*N, cudaMemcpyDeviceToHost);
  cudaDeviceSynchronize();
  gettimeofday(&t2, 0);
  double time2 = (1000000.0*(t2.tv_sec-t1.tv_sec) + t2.tv_usec-t1.tv_usec)/1000.0;
  printf("Time to generate:  %3.1f ms \n", time2);

  // check results
  for (i=0; i<N; i++) assert(a_h[i] == b_h[i]);
  // cleanup
  free(a_h); free(b_h); cudaFree(a_d);
}

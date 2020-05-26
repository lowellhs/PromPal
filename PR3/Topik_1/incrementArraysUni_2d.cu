// incrementArray.cu

#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

void incrementArrayOnHost(float *a, unsigned long N)
{
  unsigned long i;
  for (i=0; i < N; i++) a[i] = a[i]+1.f;
}

__global__ void incrementArrayOnDevice(float *a, unsigned long N)
{
  unsigned long idx = blockIdx.x*blockDim.x + threadIdx.x;
  unsigned long idy = blockIdx.y*blockDim.y + threadIdx.y;
  unsigned long id  = idy*gridDim.x*blockDim.x + idx;
  if (id<N) a[id] = a[id]+1.f;
}

int main(int argc, char **argv)
{
  float *a, *b;
  unsigned long i, N = strtoul(argv[1], NULL, 10);
  size_t size = N*sizeof(float);

  gpuErrchk( cudaMallocManaged((void **) &a, size) );
  gpuErrchk( cudaMallocManaged((void **) &b, size) );

  for (i=0; i<N; i++)
  {
    a[i] = (float)i;
    b[i] = (float)i;
  }
  
  // do calculation on host
  incrementArrayOnHost(a, N);

  // do calculation on device:
  // Part 1 of 2. Compute execution configuration
  unsigned long blockDimX = strtoul(argv[2], NULL, 10);
  unsigned long blockDimY = strtoul(argv[3], NULL, 10);
  unsigned long gridDimX = strtoul(argv[4], NULL, 10);
  unsigned long gridDimY = strtoul(argv[5], NULL, 10);

  dim3 blockSize = dim3(blockDimX, blockDimY);
  dim3 gridSize = dim3(gridDimX, gridDimY);
  printf("blockDim: (%lu,%lu), gridDim: (%lu,%lu)\n", blockDimX, blockDimY, gridDimX, gridDimY);

  // Part 2 of 2. Call incrementArrayOnDevice kernel
  incrementArrayOnDevice <<< gridSize, blockSize >>> (b, N);
  cudaDeviceSynchronize();
  
  // check results
  for (i=0; i<N; i++) assert(a[i] == b[i]);

  // cleanup
  cudaFree(a); cudaFree(b);
}

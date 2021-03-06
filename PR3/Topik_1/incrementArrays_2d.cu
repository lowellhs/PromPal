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
  float *a_h, *b_h; // pointers to host memory
  float *a_d; // pointer to device memory
  unsigned long i, N = strtoul(argv[1], NULL, 10);
  size_t size = N*sizeof(float);

  // allocate arrays on host
  a_h = (float *)malloc(size);
  b_h = (float *)malloc(size);

  // allocate array on device
  gpuErrchk( cudaMalloc((void **) &a_d, size) );
  // initialization of host data
  for (i=0; i<N; i++) a_h[i] = (float)i;
  // copy data from host to device
  gpuErrchk( cudaMemcpy(a_d, a_h, sizeof(float)*N, cudaMemcpyHostToDevice) );
  
  // do calculation on host
  incrementArrayOnHost(a_h, N);

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
  incrementArrayOnDevice <<< gridSize, blockSize >>> (a_d, N);
  // Retrieve result from device and store in b_h
  gpuErrchk( cudaMemcpy(b_h, a_d, sizeof(float)*N, cudaMemcpyDeviceToHost) );

  // cudaError err = cudaGetLastError();
  // if ( cudaSuccess != err ) printf("cudaCheckError() failed: %s\n", cudaGetErrorString(err));
  cudaDeviceSynchronize();
  
  // check results
  for (i=0; i<N; i++) assert(a_h[i] == b_h[i]);

  // cleanup
  free(a_h); free(b_h); cudaFree(a_d);
}

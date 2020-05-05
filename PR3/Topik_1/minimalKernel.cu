#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>


__global__ void kernelId(int *a)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  a[idx] = idx;
}

__global__ void kernelBlockIdx(int *a)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  a[idx] = blockIdx.x;
}

__global__ void kernelThreadIdx(int *a)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  a[idx] = threadIdx.x;
}

int main(void)
{
  int *a_h, *b_h; //pointers to host memory
  int *a_d; //pointers to device memory
  int i;
  int N = 20;
  size_t size = N*sizeof(int);
   
  //allocate array on host
  a_h = (int *)malloc(size);
  b_h = (int *)malloc(size);

  //allocate array on device
  cudaMalloc((void **) &a_d, size);
  
  //initialization of host data
  for (i=0; i<N; i++) a_h[i] = 0;

  //copy data from host to device
  cudaMemcpy(a_d, a_h, sizeof(int)*N, cudaMemcpyHostToDevice);
  
  //do calculation on host
  //kernelId<<<5,4>>>(a_d);
  //kernelBlockIdx<<<10,2>>>(a_d);
  kernelThreadIdx<<<10,2>>>(a_d);

  //retrieve result from device and store in b_h
  cudaMemcpy(b_h, a_d, sizeof(int)*N, cudaMemcpyDeviceToHost);
  
  //print out the result
  for (i=0; i<N; i++) printf("%d", b_h[i]);
  printf("\n");

  //cleanup
  free(a_h); free(b_h); cudaFree(a_d);
}

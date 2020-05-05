#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>


__global__ void kernel(int *a)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  a[idx] = 7;
}

int main(void)
{
  int *a_h, *b_h; //pointers to host memory
  int *a_d; //pointers to device memory
  int i;
  int N = 10;
  
  //allocate array on host
  a_h = (int *)malloc(sizeof(int));
  b_h = (int *)malloc(sizeof(int));

  //allocate array on device
  cudaMalloc((void **) &a_d, sizeof(int));
  
  //initialization of host data
  for (i=0; i<N; i++) a_h[i] = 0;

  //copy data from host to device
  cudaMemcpy(a_d, a_h, sizeof(int)*N, cudaMemcpyHostToDevice);
  
  //do calculation on host
  kernel<<<1,1>>>(a_h);

  //retrieve result from device and store in b_h
  cudaMemcpy(b_h, a_d, sizeof(int)*N, cudaMemcpyDeviceToHost);
  
  //print out the result
  for (i=0; i<N; i++) printf("%d", b_h[i]);

  //cleanup
  free(a_h); free(b_h); cudaFree(a_d);
}

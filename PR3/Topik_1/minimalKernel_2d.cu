#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>


__global__ void kernelId(int *a)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  int idy = blockIdx.y*blockDim.y + threadIdx.y;
  int id = idy*blockDim.x*gridDim.x + idx;
  a[id] = id;
}

__global__ void kernelBlockIdxX(int *a)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  int idy = blockIdx.y*blockDim.y + threadIdx.y;
  int id = idy*blockDim.x*gridDim.x + idx;
  a[id] = blockIdx.x;
}

__global__ void kernelBlockIdxY(int *a)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  int idy = blockIdx.y*blockDim.y + threadIdx.y;
  int id = idy*blockDim.x*gridDim.x + idx;
  a[id] = blockIdx.y;
}

__global__ void kernelThreadIdxX(int *a)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  int idy = blockIdx.y*blockDim.y + threadIdx.y;
  int id = idy*blockDim.x*gridDim.x + idx;
  a[id] = threadIdx.x;
}

__global__ void kernelThreadIdxY(int *a)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  int idy = blockIdx.y*blockDim.y + threadIdx.y;
  int id = idy*blockDim.x*gridDim.x + idx;
  a[id] = threadIdx.y;
}

void printArr(int *a, int N)
{
  for (int i=0; i<N; i++) printf("%2d ", a[i]);
  printf("\n");
}

int main(int argc, char **argv)
{
  int N = atoi(argv[1]);
  for (int k=0; k<5; k++)
  {
    int *a_h, *b_h; //pointers to host memory
    int *a_d; //pointers to device memory
    int i;
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
    dim3 dimGrid = dim3(atoi(argv[2]), atoi(argv[3]));
    dim3 dimBlock = dim3(atoi(argv[4]), atoi(argv[5]));

    if (k==0) kernelId<<<dimGrid,dimBlock>>>(a_d);
    if (k==1) kernelBlockIdxX<<<dimGrid,dimBlock>>>(a_d);
    if (k==2) kernelBlockIdxY<<<dimGrid,dimBlock>>>(a_d);
    if (k==3) kernelThreadIdxX<<<dimGrid,dimBlock>>>(a_d);
    if (k==4) kernelThreadIdxY<<<dimGrid,dimBlock>>>(a_d);

    //retrieve result from device and store in b_h
    cudaMemcpy(b_h, a_d, sizeof(int)*N, cudaMemcpyDeviceToHost);
    
    //print out the result
    printArr(b_h, N);

    //cleanup
    free(a_h); free(b_h); cudaFree(a_d);
  }
}

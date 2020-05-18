#include <stdio.h>
#include <stdlib.h>
#include "matrixTools.c"
#include <sys/time.h>
#include <cuda.h>

#define TOL 1e-6
#define limit_iter 10000

__device__ int flag;

__global__ void jacobiOnDevice(int n, float *A, float *b, float *x_iter, float *x_iter_new)
{
  float sigma = 0;
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < n)
  {
    for (int j=0; j<n; j++) {
      if (j != i) {
        sigma = sigma + A[i*n+j]*x_iter[j];
      }
    }
    x_iter_new[i] = (b[i]-sigma)/A[i*n+i];
  }
}

__global__ void checkConvergence(int n, float *x_iter, float *x_iter_new)
{
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i<n)
  {
    if (fabs(x_iter[i]-x_iter_new[i]) > TOL) flag = 0;
    x_iter[i] = x_iter_new[i];
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
      
      float *A, *b, *x, *x_iter, *x_iter_new;

      int n = atoi(argv[counter]);
      cudaMallocManaged(&A, n*n*sizeof(float));
      cudaMallocManaged(&b, n*sizeof(float));
      cudaMallocManaged(&x, n*sizeof(float));
      cudaMallocManaged(&x_iter, n*sizeof(float));
      cudaMallocManaged(&x_iter_new, n*sizeof(float));
      
      char file_A[80], file_b[80], file_x[80];
      sprintf(file_A, "test_input/matrix_A_%dx%d.txt", n, n);
      sprintf(file_b, "test_input/vector_b_%dx1.txt", n);
      sprintf(file_x, "test_input/vector_x_%dx1.txt", n);
      read_matrix(n, A, file_A, " ");
      read_vector(n, b, file_b, " ");
      read_vector(n, x, file_x, " ");
      for (int i=0; i<n; i++) x_iter[i] = 0.0;
    
      cudaEventCreate(&start);
      cudaEventCreate(&stop);
      cudaEventRecord(start);
      int k = 0, isConverged;
      do {
        isConverged = 1;

        cudaMemcpyToSymbol(flag, &isConverged, sizeof(int));
        jacobiOnDevice<<<dim3((int)ceil(n/1024.0),1,1), dim3(1024,1,1)>>>(n, A, b, x_iter, x_iter_new);
        checkConvergence<<<dim3((int)ceil(n/1024.0),1,1), dim3(1024,1,1)>>>(n, x_iter, x_iter_new);
        cudaMemcpyFromSymbol(&isConverged, flag, sizeof(int));
        cudaDeviceSynchronize();

        k++;
      } while (k < limit_iter && !isConverged);

      cudaEventRecord(stop);
      cudaEventSynchronize(stop);
      float milliseconds = 0; cudaEventElapsedTime(&milliseconds, start, stop);
      cudaDeviceSynchronize();

      // cudaError_t code=cudaGetLastError();
      // printf("%s\n", cudaGetErrorString(code));
      printf("%d ", n);
      printf("%.6f ", milliseconds*1e-3);
      printf("%.9f ", norm_vector(n, x_iter, x));
      printf("%d\n", k);
      cudaFree(A); cudaFree(b); cudaFree(x); cudaFree(x_iter); cudaFree(x_iter_new);
    }
  }
  return 0;
}

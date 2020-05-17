#include <stdio.h>
#include <stdlib.h>
#include "matrixTools.c"
#include <sys/time.h>
#include <cuda.h>

#define TOL 1e-6
#define limit_iter 10000

__device__ int flag;

__global__ void jacobiOnDevice(int n, float *A, float *b, float *x_iter)
{
  float sigma = 0, newValue;
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < n)
  {
    for (int j=0; j<n; j++) {
      if (j != i) {
        sigma = sigma + A[i*n+j]*x_iter[j];
      }
    }
    newValue = (b[i]-sigma)/A[i*n+i];
    __syncthreads();
    if (fabs(x_iter[i]-newValue) > TOL) flag = 0;
    x_iter[i] = newValue;
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
      
      float *A, *b, *x, *x_iter;
      float *A_d, *b_d, *x_iter_d;

      int n = atoi(argv[counter]);
      A = (float *)malloc(sizeof(float)*n*n);
      b = (float *)malloc(sizeof(float)*n);
      x = (float *)malloc(sizeof(float)*n);
      x_iter = (float *)malloc(sizeof(float)*n);

      cudaMalloc((void **) &A_d, sizeof(float)*n*n);
      cudaMalloc((void **) &b_d, sizeof(float)*n);
      cudaMalloc((void **) &x_iter_d, sizeof(float)*n);
      
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

      cudaMemcpy(A_d, A, n*n*sizeof(float), cudaMemcpyHostToDevice);
      cudaMemcpy(b_d, b, n*sizeof(float), cudaMemcpyHostToDevice);
      cudaMemcpy(x_iter_d, x_iter, n*sizeof(float), cudaMemcpyHostToDevice);
      cudaDeviceSynchronize();


      int k = 0, isConverged;
      do {
        isConverged = 1;

        cudaMemcpyToSymbol(flag, &isConverged, sizeof(int));
        jacobiOnDevice<<<dim3((int)ceil(n/1024.0),1,1),dim3(1024,1,1)>>>(n, A_d, b_d, x_iter_d);
        cudaMemcpyFromSymbol(&isConverged, flag, sizeof(int));
        cudaDeviceSynchronize();

        k++;
      } while (k < limit_iter && !isConverged);

      cudaMemcpy(x_iter, x_iter_d, n*sizeof(float), cudaMemcpyDeviceToHost);

      cudaEventRecord(stop);
      cudaEventSynchronize(stop);
      float milliseconds = 0; cudaEventElapsedTime(&milliseconds, start, stop);
      cudaDeviceSynchronize();

      printf("%d ", n);
      printf("%.6f ", milliseconds*1e-3);
      printf("%.9f ", norm_vector(n, x_iter, x));
      printf("%d\n", k);
      free(A); free(b); free(x); free(x_iter);
      cudaFree(A_d); cudaFree(b_d); cudaFree(x_iter_d);
    }
  }
  return 0;
}

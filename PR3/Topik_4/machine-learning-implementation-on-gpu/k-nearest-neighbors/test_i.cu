#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cuda.h>
#include <math.h>
#include "cublas_v2.h"
#include "helper.c"

#define MAX_LEN 1000

__global__ void parallel_sort(int t, int m, int k, float *matrix, int *result)
{
  int idx = blockIdx.x*blockDim.x + threadIdx.x;
  if (idx < t)
  {
    for (int i=0; i<k; i++)
    {
      int min_idx = -1, min_idx_g = -1;
      float min_val;
      for (int j=0; j<m; j++)
      {
        int valid = 1;
        for (int jj=0; jj<i; jj++) valid = valid && (j != result[idx*k+jj]);
        if (valid)
        {
          float val = matrix[idx*m+j];
          if ((min_idx == -1) || (val < min_val))
          {
            min_val = val;
            min_idx = idx*m+j;
            min_idx_g = j;
          }
        }
      }
      result[idx*k+i] = min_idx_g;
    }
  }
}

int main(int argc, char **argv)
{
  srand(time(NULL));
  int m = atoi(argv[1]), t = atoi(argv[2]), k = atoi(argv[3]);
  float *matrix = mallocUni(t*m);
  int *result = mallocUni_int(t*k);
  for (int i=0; i<t; i++)
  {
    for (int j=0; j<m; j++)
    {
      matrix[i*m+j] = m-j;
    }
  }
  printf("Begin...\n");
  cudaDeviceSynchronize();
  parallel_sort<<<(int)ceil((t)/1024.0), 1024>>>(t, m, k, matrix, result);
  cudaDeviceSynchronize();
  printf("Done!\n");
  // print_matrix(t, m, matrix);
  print_matrix_int(t, k, result);
  freeUni(matrix);
  freeUni_int(result);
  return 0;
}

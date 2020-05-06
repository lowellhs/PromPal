#include <stdio.h>
#include <assert.h>
#include <cuda.h>
#include <math.h>

void matmul(int n, float *A, float *B, float *C)
{
  for (int k=0; k<n; k++) {
    for (int j=0; j<n; j++) {
      C[n*k+j] = 0;
      for (int i=0; i<n; i++) {
        C[n*k+j] = C[n*k+j] + A[n*k+i] * B[n*i+j];
      }
    }
  }
}

int main(int argc, char **argv)
{
  int n = 8;
  size_t size = n*n*sizeof(float);

  float *A = (float *)malloc(size);
  float *B = (float *)malloc(size);
  float *C = (float *)malloc(size);

  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      A[n*i+j] = (float)n*i+j;
      B[n*i+j] = (float)n*i+j;
      C[n*i+j] = 0.0;
    }
  }

  matmul(n, A, B, C);

  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      printf("%10.4f ", C[n*i+j]);
    }
    printf("\n");
  }

  free(A);
  free(B);
  free(C);

}

#include <stdio.h>
#include <stdlib.h>
#include "matrixTools.c"
#include <sys/time.h>

#define TOL 1e-6
#define limit_iter 10000

int main(int argc, char **argv)
{
  int sizeCounter = atoi(argv[1]);
  int tests = atoi(argv[2]);
  for (int counter=3; counter < 3+sizeCounter; counter++)
  {
    for (int testCounter=0; testCounter < tests; testCounter++)
    {
      struct timeval startCPU, stopCPU;
      float *A, *b, *x, *x_iter, *x_iter_new;
      float dist;

      int n = atoi(argv[counter]);
      A = (float *)malloc(sizeof(float)*n*n);
      b = (float *)malloc(sizeof(float)*n);
      x = (float *)malloc(sizeof(float)*n);
      x_iter = (float *)malloc(sizeof(float)*n);
      x_iter_new = (float *)malloc(sizeof(float)*n);
      
      char file_A[80], file_b[80], file_x[80];
      sprintf(file_A, "test_input/matrix_A_%dx%d.txt", n, n);
      sprintf(file_b, "test_input/vector_b_%dx1.txt", n);
      sprintf(file_x, "test_input/vector_x_%dx1.txt", n);
      read_matrix(n, A, file_A, " ");
      read_vector(n, b, file_b, " ");
      read_vector(n, x, file_x, " ");
      for (int i=0; i<n; i++) x_iter[i] = 0.0;

      gettimeofday(&startCPU, 0);
      int k = 0, isConverged;
      do {
        isConverged = 1;
        for (int i=0; i<n; i++) {
          x_iter_new[i] = b[i];
          for (int j=0; j<n; j++) {
            if (j != i) {
              x_iter_new[i] -= (A[i*n+j] * x_iter[j]);
            }
          }
          x_iter_new[i] /= A[i*n+i];
        }

        for (int i=0; i<n; i++) {
          if (fabs(x_iter[i]-x_iter_new[i]) > TOL) isConverged = 0;
          x_iter[i] = x_iter_new[i];
        }
        printVector(n, x_iter); printf("\n");
        k++;
      } while (k < limit_iter && !isConverged);
      gettimeofday(&stopCPU, 0);

      printf("%d ", n);
      printf("%.6f ", (stopCPU.tv_sec+stopCPU.tv_usec*1e-6)-(startCPU.tv_sec+startCPU.tv_usec*1e-6));
      printf("%.9f ", norm_vector(n, x_iter, x));
      printf("%d\n", k);
      free(A); free(b); free(x); free(x_iter); free(x_iter_new);
    }
  }
  return 0;
}


#include "matrixTools.c"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
  int sizeCounter = atoi(argv[1]);
  int tests = atoi(argv[2]);
  for (int counter=3; counter < 3+sizeCounter; counter++)
  {
    for (int testCounter=0; testCounter < tests; testCounter++)
    {
    struct timeval startCPU, stopCPU;

    float *A_h, *B_h, *C_h;

    // size of matrix (n*n)
    int n = atoi(argv[counter]);
    size_t size = n*n*sizeof(float);

    // allocate array on host
    A_h  = (float *)malloc(size);
    B_h  = (float *)malloc(size);
    C_h  = (float *)malloc(size);

    // initializtion of host data
    initIdentityMatrix(n, A_h);
    initRandomMatrix(n, B_h);

    gettimeofday(&startCPU, 0);
    matmul(n, A_h, B_h, C_h);
    gettimeofday(&stopCPU, 0);

    float err = errorMatrix(n, C_h, B_h);
    printf("%d ", n);
    printf("%.6f ", (stopCPU.tv_sec+stopCPU.tv_usec*1e-6)-(startCPU.tv_sec+startCPU.tv_usec*1e-6));
    printf("%.6f\n", err);

    // Cleanup
    free(A_h); free(B_h); free(C_h);
    }
  }
}


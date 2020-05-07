#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "matrixTools.c"

MPI_Status status;

int main(int argc, char **argv)
{
  int num_procs, my_rank, num_workers, rows, i, j, k;
  double start, stop;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int n = atoi(argv[1]);
  float *A  = (float *)malloc(n*n*sizeof(float));
  float *B  = (float *)malloc(n*n*sizeof(float));
  float *C  = (float *)malloc(n*n*sizeof(float));

  if (my_rank == 0)
  {
    initIdentityMatrix(n, A);
    initRandomMatrix(n, B);
    //printMatrix(n, A);
    //printMatrix(n, B);
  }

  start = MPI_Wtime();
  rows = n / num_procs;
  float *recA  = (float *)malloc(rows*n*sizeof(float));
  float *sendC = (float *)malloc(rows*n*sizeof(float));

  MPI_Bcast(&(B[0]), n*n, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Scatter(&(A[0]), rows*n, MPI_FLOAT, &(recA[0]), rows*n, MPI_FLOAT, 0, MPI_COMM_WORLD);


  for (k=0; k<rows; k++) {
    for (j=0; j<n; j++) {
      sendC[k*n+j] = 0;
      for (i=0; i<n; i++) {
        sendC[k*n+j] = sendC[k*n+j] + recA[k*n+i] * B[i*n+j];
      }
    }
  }
  if (my_rank == 0) {
    for (k=rows*num_procs; k<n; k++) {
      for (j=0; j<n; j++) {
        C[k*n+j] = 0;
        for (i=0; i<n; i++) {
          C[k*n+j] = C[k*n+j] + A[k*n+i] * B[i*n+j];
        }
      }
    }
  }

  MPI_Gather(&(sendC[0]), rows*n, MPI_FLOAT, &(C[0]), rows*n, MPI_FLOAT, 0, MPI_COMM_WORLD);
  stop = MPI_Wtime();
  MPI_Finalize();

  if (my_rank == 0) {
    float err = errorMatrix(n, B, C);
    printf("TOTAL TIME : %.6f s\n", stop - start);
    printf("err : %.6f\n", err);
  }

  free(A); free(B); free(C);
  free(recA); free(sendC);
  return 0;
}

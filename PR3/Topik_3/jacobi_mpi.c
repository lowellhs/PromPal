#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "matrixTools.c"

#define TOL 1e-6
#define limit_iter 10000

MPI_Status status;

int main(int argc, char **argv)
{
  int num_procs, my_rank, num_workers, rows, i, j;
  double start, stop;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  float *A, *b, *x, *x_iter, *x_iter_new;

  int n = atoi(argv[1]);
  A = (float *)malloc(sizeof(float)*n*n);
  b = (float *)malloc(sizeof(float)*n);
  x = (float *)malloc(sizeof(float)*n);
  x_iter = (float *)malloc(sizeof(float)*n);
  x_iter_new = (float *)malloc(sizeof(float)*n);

  if (my_rank == 0) {
    char file_A[80], file_b[80], file_x[80];
    sprintf(file_A, "test_input/matrix_A_%dx%d.txt", n, n);
    sprintf(file_b, "test_input/vector_b_%dx1.txt", n);
    sprintf(file_x, "test_input/vector_x_%dx1.txt", n);
    read_matrix(n, A, file_A, " ");
    read_vector(n, b, file_b, " ");
    read_vector(n, x, file_x, " ");
    for (int i=0; i<n; i++) x_iter[i] = 0.0;
  }

  start = MPI_Wtime();
  rows = n / num_procs;
  float *recA  = (float *)malloc(rows*n*sizeof(float));
  float *recb = (float *)malloc(rows*sizeof(float));
  float *sendx = (float *)malloc(rows*sizeof(float));

  MPI_Bcast(&(x_iter[0]), n, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Scatter(&(A[0]), rows*n, MPI_FLOAT, &(recA[0]), rows*n, MPI_FLOAT, 0, MPI_COMM_WORLD);
  MPI_Scatter(&(b[0]), rows, MPI_FLOAT, &(recb[0]), rows, MPI_FLOAT, 0, MPI_COMM_WORLD);

  int isConverged;
  int k = 0;
  do {
    isConverged = 1;
    for (i=0; i<rows; i++) {
      sendx[i] = recb[i];
      for (j=0; j<n; j++) {
        if ((my_rank*rows+i) != j) {
          sendx[i] -= (recA[i*rows+j] * x_iter[j]);
        }
      }
      sendx[i] /= recA[i*rows+(my_rank*rows+i)];
    }
    MPI_Allgather(&(sendx[0]), rows, MPI_FLOAT, &(x_iter_new[0]), rows, MPI_FLOAT, MPI_COMM_WORLD);
    for (int i=0; i<n; i++) {
      if (fabs(x_iter[i]-x_iter_new[i]) > TOL) isConverged = 0;
      x_iter[i] = x_iter_new[i];
    }
    k++;
  } while (k < limit_iter && !isConverged);

  stop = MPI_Wtime();
  MPI_Finalize();
	
  if (my_rank == 0) {
    printf("%d ", n);
    printf("%.6f ", stop-start);
    printf("%.9f ", norm_vector(n, x_iter, x));
    printf("%d\n", k);
  }

  return 0;
}
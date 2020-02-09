#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define ROWS 10
#define COLS 10
#define ROWS_A ROWS
#define COLS_A COLS
#define VECSIZE ROWS

int main(void) {
  int     num_procs,
          my_rank,
          num_workers,
          source,
          dest,
          rows,
          rows_per_task, rem_rows, offset,
          i, j;

  double  A[ROWS_A][COLS_A],
          x[VECSIZE],
          b[VECSIZE];

  MPI_Status status;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  // 1 process for master, (num_procs - 1) remaining
  num_workers = num_procs - 1;

  if (my_rank == 0) {
    // Initialize matrix A
    for (i=0; i<ROWS_A; i++) {
      for (j=0; j<COLS_A; j++) {
        A[i][j] = i + j;
      }
    }
    // Initialize vector x
    for (i=0; i<VECSIZE; i++) {
      x[i] = i;
    }
    // Initialize vector b
    for (i=0; i<VECSIZE; i++) {
      b[i] = 0;
    }

    offset = 0;
    rows_per_task = ROWS_A / num_workers;
    rem_rows = ROWS_A % num_workers;

    for (dest=1; dest<=num_workers; dest++) {
      rows = rows_per_task;
      if (dest == num_workers) {
        rows = rows + rem_rows;
      }

      MPI_Send(&offset, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&A[offset][0], rows*COLS_A, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&x, VECSIZE, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);

      offset = offset + rows;
    }

    for (source=1; source<=num_workers; source++) {
      MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&b[offset], rows, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
    }
    for (i=0; i<VECSIZE; i++) {
      printf("%f\n", b[i]);
    }
    printf("%s\n", "");
  } 
  
  if (my_rank > 0) {
      MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&A[offset][0], rows*COLS_A, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&x, VECSIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

      for (i=offset; i<offset+rows; i++) {
        b[i] = 0.0;
        for (j=0; j<COLS_A; j++) {
          b[i] = b[i] + A[i][j] * x[j];
        }
      }

      MPI_Send(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&b[offset], rows, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
  }
  MPI_Finalize();
  return 0;
}
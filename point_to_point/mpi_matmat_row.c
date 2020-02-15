#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init.c"

MPI_Status status;
double A[n][n], B[n][n], C[n][n];

int main(int argc, char **argv) {
  int num_procs, my_rank, num_workers, source, dest, rows, rows_per_task, rem_rows, offset;
  int i, j, k, rc, from_file, print_flag;
  double start, start_node, start_calc;
  double stop, stop_node, stop_calc;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  double times[num_procs];
  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  num_workers = num_procs - 1;
  if (my_rank == 0) {
    if (from_file) {
      read_matrix(n, n, A, argv[3], " ");
      read_matrix(n, n, B, argv[4], " ");
    } else {
      if (print_flag) {
        printf("Perkalian matriks (%dx%d) A . I = C\n", n, n);
      }
      init_matrix(n, n, A);
      init_matrix_i(n, B);
    }

    start = MPI_Wtime();
    offset = 0;
    rows_per_task = n / num_workers;
    rem_rows = n % num_workers;
    for (dest=1; dest<=num_workers; dest++) {
      rows = rows_per_task;
      if (dest == num_workers) {
        rows = rows + rem_rows;
      }
      times[dest] = MPI_Wtime();
      MPI_Send(&offset, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&A[offset][0], rows*n, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&B, n*n, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
      offset = offset + rows;
    }

    for (source=1; source<=num_workers; source++) {
      MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&C[offset], rows*n, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
      times[source] = MPI_Wtime() - times[source];
    }
    stop = MPI_Wtime();
  } 
  
  if (my_rank > 0) {
    start_node = MPI_Wtime();
    MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&A[offset][0], rows*n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&B, n*n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

    start_calc = MPI_Wtime();
    for (k=offset; k<offset+rows; k++) {
      for (j=0; j<n; j++) {
        C[k][j] = 0;
        for (i=0; i<n; i++) {
          C[k][j] = C[k][j] + A[k][i] * B[i][j];
        }
      }
    }
    stop_calc = MPI_Wtime();

    MPI_Send(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    MPI_Send(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    MPI_Send(&C[offset], rows*n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    stop_node = MPI_Wtime();

    if (print_flag) {
      printf(
        "Process %d, time elapse: %f s, calculation time: %f s\n",
        my_rank,
        (stop_node-start_node), 
        (stop_calc-start_calc)
      );
    }
  }
  MPI_Finalize();

  if (my_rank == 0) {
    if (!print_flag) {
      print_matrix(n, n, C);
    } else {
      for(i=1; i<num_procs; i++) {
        printf("Send-Receive time process 0 with process %d is %f s\n", i, times[i]);
      }
      printf("TOTAL TIME : %.9f s\n", stop - start);
    }
  }
  return 0;
}
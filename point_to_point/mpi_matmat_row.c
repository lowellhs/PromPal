#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init.c"

MPI_Status status;
double A[n][n], B[n][n], C[n][n];

int main(int argc, char **argv) {
  int num_procs, my_rank, num_workers, source, dest, rows, rows_per_task, rem_rows, offset;
  int i, j, k, rc, from_file, print_flag, detail_time_flag;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  detail_time_flag = atoi(argv[3]);
  num_workers = num_procs - 1;
  if (my_rank == 0) {
    if (from_file) {
      read_matrix(n, n, A, argv[4], " ");
      read_matrix(n, n, B, argv[5], " ");
    } else {
      printf("Perkalian matriks (%dx%d) A . I = C\n...\n", n, n);
      init_matrix(n, n, A);
      init_matrix_i(n, B);
    }
    struct timeval starts[num_procs], stops[num_procs];

    gettimeofday(&starts[0], 0);
    offset = 0;
    rows_per_task = n / num_workers;
    rem_rows = n % num_workers;
    for (dest=1; dest<=num_workers; dest++) {
      rows = rows_per_task;
      if (dest == num_workers) {
        rows = rows + rem_rows;
      }
      gettimeofday(&starts[dest], 0);
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
      gettimeofday(&stops[source], 0);
    }
    gettimeofday(&stops[0], 0);

    if (print_flag == 1) {
      print_matrix(n, n, C);
    }
    if (from_file == 0) {
      char *equals_A_C = matrix_equals(n, n, A, C) ? "sama" : "berbeda";
      printf("Perkalian A . I = C menghasilkan C %s dengan A\n", equals_A_C);
    }
    printf("Total time is %.9f s\n", time_elapsed(stops[0], starts[0]));
    printf("Average communication time is %.9f s\n", avg_time_elapsed(1, num_procs, stops, starts));
    if (detail_time_flag == 1) {
      char *str_b = "Process %d start after %.9f s, done in %.9f s\n";
      for (i=1; i<num_procs; i++) {
        printf(str_b, i, time_elapsed(starts[i], starts[0]), time_elapsed(stops[i], starts[i]));
      }
    }
  } 
  
  if (my_rank > 0) {
      MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&A[offset][0], rows*n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&B, n*n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

      for (k=offset; k<offset+rows; k++) {
        for (j=0; j<n; j++) {
          C[k][j] = 0;
          for (i=0; i<n; i++) {
            C[k][j] = C[k][j] + A[k][i] * B[i][j];
          }
        }
      }

      MPI_Send(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&C[offset], rows*n, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
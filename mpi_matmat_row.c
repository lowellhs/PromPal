#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "file_reader.c"

int main(int argc, char *argv[]) {
  if (argc != 9) {
    return 0;
  };

  int n = atoi(argv[2]);
  char *file_matrix1 = argv[4];
  char *file_matrix2 = argv[6];
  int print_flag = atoi(argv[8]);
  char *delim = " ";

  double A[n][n];
  double B[n][n];
  double C[n][n];

  int num_procs,                          // Banyak proses yang ada
      my_rank,                            // id proses
      num_workers,                        // Banyak proses selain master (selain id = 0)
      source,                             // id proses sumber saat Recv
      dest,                               // id proses tujuan saat Send 
      rows,                               // Ukuran chunk matriks yang dikirim ke masing2 proses
      rows_per_task, rem_rows, offset,    // Variabel bantuan
      i, j, k;                            // Keperluan indexing

  MPI_Status status;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  // 1 process for master, (num_procs - 1) remaining
  num_workers = num_procs - 1;

  if (my_rank == 0) {
    read_matrix(n, n, A, file_matrix1, delim);
    read_matrix(n, n, B, file_matrix2, delim);

    if (print_flag == 1) {
      print_matrix(n, n, A);
      print_matrix(n, n, B);
    }

    double start = MPI_Wtime();
    offset = 0;
    rows_per_task = n / num_workers;
    rem_rows = n % num_workers;

    for (dest=1; dest<=num_workers; dest++) {
      rows = rows_per_task;
      if (dest == num_workers) {
        rows = rows + rem_rows;
      }

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
    }

    double total_time = MPI_Wtime() - start;
    if (print_flag == 1) {
      print_matrix(n, n, C);
    }
    printf("Done in %f seconds.\n", total_time);
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
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "file_reader.c"

int main(int argc, char *argv[]) {
  if (argc != 9) {
    return 0;
  };

  int n = atoi(argv[2]);
  char *file_matrix = argv[4];
  char *file_vector = argv[6];
  int print_flag = atoi(argv[8]);
  char *delim = " ";

  double A[n][n];
  double x[n];
  double b[n];

  int ROWS_A = n;   // Ukuran baris matriks A (n x n)
  int COLS_A = n;   // Ukuran kolom matriks A (n x n)
  int VECSIZE = n;  // Ukuran vektor x dan b (n x 1)

  int num_procs,                          // Banyak proses yang ada
      my_rank,                            // id proses
      num_workers,                        // Banyak proses selain master (selain id = 0)
      source,                             // id proses sumber saat Recv
      dest,                               // id proses tujuan saat Send 
      rows,                               // Ukuran chunk matriks yang dikirim ke masing2 proses
      rows_per_task, rem_rows, offset,    // Variabel bantuan
      i, j;                               // Keperluan indexing

  MPI_Status status;

  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  // 1 process for master, (num_procs - 1) remaining
  num_workers = num_procs - 1;

  if (my_rank == 0) {
    read_matrix(n, A, file_matrix, delim);
    read_vector(n, x, file_vector, delim);

    if (print_flag == 1) {
      print_matrix(n, A);
      print_vector(n, x);
    }

    double start = MPI_Wtime();
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

    double total_time = MPI_Wtime() - start;
    if (print_flag == 1) {
      print_vector(n, b);
    }
    printf("Done in %f seconds.\n", total_time);
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
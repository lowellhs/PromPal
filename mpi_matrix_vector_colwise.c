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
      cols,                               // Ukuran chunk matriks yang dikirim ke masing2 proses
      cols_per_task, rem_cols, offset,    // Variabel bantuan
      i, j;                               // Keperluan indexing

  MPI_Status status;
  MPI_Datatype column_type;

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
    cols_per_task = COLS_A / num_workers;
    rem_cols = COLS_A % num_workers;

    for (dest=1; dest<=num_workers; dest++) {
      cols = cols_per_task;
      if (dest == num_workers) {
        cols = cols + rem_cols;
      }

      MPI_Send(&offset, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&cols, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Type_vector(ROWS_A, cols, COLS_A, MPI_DOUBLE, &column_type);
      MPI_Type_commit(&column_type);
      MPI_Send(&A[0][offset], 1, column_type, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&x, VECSIZE, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);

      offset = offset + cols;
    }

    for (source=1; source<=num_workers; source++) {
      double temp_b[VECSIZE];
      MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&cols, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&temp_b, VECSIZE, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
      for (i=0; i<VECSIZE; i++) {
        b[i] = temp_b[i] + b[i];
      }
    }
    
    double total_time = MPI_Wtime() - start;
    if (print_flag == 1) {
      print_vector(n, b);
    }
    printf("Done in %f seconds.\n", total_time);
  } 
  
  if (my_rank > 0) {
      MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&cols, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Type_vector(ROWS_A, cols, COLS_A, MPI_DOUBLE, &column_type);
      MPI_Type_commit(&column_type);
      MPI_Recv(&A[0][offset], ROWS_A*cols, column_type, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&x, VECSIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

      for (j=offset; j<offset+cols; j++) {
        for (i=0; i<ROWS_A; i++) {
          b[i] = b[i] + A[i][j] * x[j];
        }
      }

      MPI_Send(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&cols, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&b, VECSIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
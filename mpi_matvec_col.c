#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "file_reader.c"

int main(int argc, char *argv[]) {
  if (argc != 11) {
    return 0;
  };

  int m = atoi(argv[2]);
  int n = atoi(argv[4]);
  char *file_matrix = argv[6];
  char *file_vector = argv[8];
  int print_flag = atoi(argv[10]);
  char *delim = " ";


  int ROWS_A = m;     // Ukuran baris matriks A (n x n)
  int COLS_A = n;     // Ukuran kolom matriks A (n x n)
  int VECSIZE_x = n;  // Ukuran vektor x (n x 1)
  int VECSIZE_b = m;  // Ukuran vektor b (m x 1)

  double A[ROWS_A][COLS_A];
  double x[VECSIZE_x];
  double b[VECSIZE_b];

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
    read_matrix(m, n, A, file_matrix, delim);
    read_vector(n, x, file_vector, delim);

    if (print_flag == 1) {
      print_matrix(m, n, A);
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
      MPI_Send(&x, VECSIZE_x, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);

      offset = offset + cols;
    }

    for (source=1; source<=num_workers; source++) {
      MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&cols, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(b, VECSIZE_b, MPI_DOUBLE, source, 3, MPI_COMM_WORLD, &status);
    }
    
    double total_time = MPI_Wtime() - start;
    if (print_flag == 1) {
      print_vector(VECSIZE_b, b);
    }
    printf("Done in %f seconds.\n", total_time);
  } 
  
  if (my_rank > 0) {
      MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&cols, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Type_vector(ROWS_A, cols, COLS_A, MPI_DOUBLE, &column_type);
      MPI_Type_commit(&column_type);
      MPI_Recv(&A[0][offset], ROWS_A*cols, column_type, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&x, VECSIZE_x, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

      double temp_b[VECSIZE_b];
      for (j=offset; j<offset+cols; j++) {
        b[i] = 0;
        for (i=0; i<ROWS_A; i++) {
         b[i] = b[i] + A[i][j] * x[j];
        }
      }

      MPI_Send(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&cols, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(temp_b, VECSIZE_b, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}
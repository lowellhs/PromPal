#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "file_reader.c"

int main(int argc, char **argv) {
  int num_procs,                          // Banyak proses yang ada
      my_rank,                            // id proses
      num_workers,                        // Banyak proses selain master (selain id = 0)
      source,                             // id proses sumber saat Recv
      dest,                               // id proses tujuan saat Send 
      rows,                               // Ukuran chunk matriks yang dikirim ke masing2 proses
      rows_per_task, rem_rows, offset,    // Variabel bantuan
      i, j, rc;                           // misc
  struct timeval start, stop;

  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  int m, n, print_flag, from_file;
  if (argc == 7) {
    m = atoi(argv[2]);
    n = atoi(argv[4]);
    from_file = 0;
    print_flag = atoi(argv[6]);
  } else if (argc == 11) {
    m = atoi(argv[2]);
    n = atoi(argv[4]);
    from_file = 1;
    print_flag = atoi(argv[10]);
  } else {
    MPI_Abort(MPI_COMM_WORLD, rc);
    exit(1);
  }

  int ROWS_A = m;     // Ukuran baris matriks A (n x n)
  int COLS_A = n;     // Ukuran kolom matriks A (n x n)
  int VECSIZE_x = n;  // Ukuran vektor x (n x 1)
  int VECSIZE_b = m;  // Ukuran vektor b (m x 1)

  double A[ROWS_A][COLS_A];
  double x[VECSIZE_x];
  double b[VECSIZE_b];

  // 1 process for master, (num_procs - 1) remaining
  num_workers = num_procs - 1;

  if (my_rank == 0) {
    if (from_file) {
      char *file_matrix = argv[6];
      char *file_vector = argv[8];
      char *delim = " ";

      read_matrix(m, n, A, file_matrix, delim);
      read_vector(n, x, file_vector, delim);
    } else {
      for (i=0; i<ROWS_A; i++) {
        for (j=0; j<COLS_A; j++) {
          A[i][j] = ((i + j) % 17) + 1;
        }
      }
      for (i=0; i<VECSIZE_x; i++) {
        x[i] = i;
      }
    }

    gettimeofday(&start, 0);
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
      MPI_Send(&x, VECSIZE_x, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);

      offset = offset + rows;
    }

    for (source=1; source<=num_workers; source++) {
      MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&b[offset], rows, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
    }
    gettimeofday(&stop, 0);

    if (print_flag == 1) {
      print_vector(VECSIZE_b, b);
    } else {
      printf(
        "Done in %.6f seconds.\n", 
        (stop.tv_sec+stop.tv_usec*1e-6)-(start.tv_sec+start.tv_usec*1e-6)
      );
    }
  } 
  
  if (my_rank > 0) {
      MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&A[offset][0], rows*COLS_A, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&x, VECSIZE_x, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

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
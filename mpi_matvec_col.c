#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "helper.c"
#include "init.c"

MPI_Status status;
MPI_Datatype column_type;
double A[m][n], x[n], b[m], temp_b[m];

int main(int argc, char **argv) {
  int num_procs, my_rank, num_workers, source, dest, cols, cols_per_task, rem_cols, offset;
  int i, j, rc, from_file, print_flag;
  for (i=0; i<m; i++) {
    b[i] = 0.0;
    temp_b[i] = 0.0;
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  num_workers = num_procs - 1;
  if (my_rank == 0) {
    if (from_file) {
      read_matrix(m, n, A, argv[3], " ");
      read_vector(n, x, argv[4], " ");
    } else {
      printf("Perkalian matriks (%dx%d) vektor (%dx1) I . x = b\n...\n", n, n, n);
      init_matrix_i(n, A);
      init_vector(n, x);
    }
    struct timeval starts[num_procs], stops[num_procs];

    gettimeofday(&starts[0], 0);
    offset = 0;
    cols_per_task = n / num_workers;
    rem_cols = n % num_workers;
    for (dest=1; dest<=num_workers; dest++) {
      cols = cols_per_task;
      if (dest == num_workers) {
        cols = cols + rem_cols;
      }
      gettimeofday(&starts[dest], 0);
      MPI_Send(&offset, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&cols, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Type_vector(m, cols, n, MPI_DOUBLE, &column_type);
      MPI_Type_commit(&column_type);
      MPI_Send(&A[0][offset], 1, column_type, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&x, n, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
      offset = offset + cols;
    }

    for (source=1; source<=num_workers; source++) {
      MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&cols, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&temp_b, m, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
      gettimeofday(&stops[source], 0);
      for (i=0; i<m; i++) {
        b[i] = b[i] + temp_b[i];
      }
    }
    gettimeofday(&stops[0], 0);

    if (print_flag == 1) {
      print_vector(m, b);
    }
    if (from_file == 0) {
      char *equals_x_b = vector_equals(n, x, b) ? "sama" : "berbeda";
      printf("Perkalian I . x = b menghasilkan b %s dengan x\n", equals_x_b);
    }
    char *str_a = "Total time is %.9f seconds.\n";
    char *str_b = "Process %d start after %.9f seconds, done in %.9f seconds.\n";
    printf(str_a, time_elapsed(stops[0], starts[0]));
    for (i=1; i<num_procs; i++) {
      printf(str_b, i, time_elapsed(starts[i], starts[0]), time_elapsed(stops[i], starts[i]));
    }
  } 
  
  if (my_rank > 0) {
      MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&cols, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Type_vector(m, cols, n, MPI_DOUBLE, &column_type);
      MPI_Type_commit(&column_type);
      MPI_Recv(&A[0][offset], m*cols, column_type, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&x, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

      for (j=offset; j<offset+cols; j++) {
        for (i=0; i<m; i++) {
          b[i] = b[i] + A[i][j] * x[j];
        }
      }

      MPI_Send(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&cols, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(&b, m, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "helper.c"
#include "init.c"

MPI_Status status;
double A[m][n], x[n], b[m];

int main(int argc, char **argv) {
  int num_procs, my_rank, num_workers, source, dest, rows, rows_per_task, rem_rows, offset;
  int i, j, rc, from_file, print_flag;

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
    rows_per_task = m / num_workers;
    rem_rows = m % num_workers;
    for (dest=1; dest<=num_workers; dest++) {
      rows = rows_per_task;
      if (dest == num_workers) {
        rows = rows + rem_rows;
      }
      gettimeofday(&starts[dest], 0);
      MPI_Send(&offset, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&rows, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&A[offset][0], rows*n, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
      MPI_Send(&x, n, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);
      offset = offset + rows;
    }

    for (source=1; source<=num_workers; source++) {
      MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&b[offset], rows, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, &status);
      gettimeofday(&stops[source], 0);
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
      MPI_Recv(&rows, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&A[offset][0], rows*n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&x, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

      for (i=offset; i<offset+rows; i++) {
        b[i] = 0.0;
        for (j=0; j<n; j++) {
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
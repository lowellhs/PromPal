#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"
#include "../init.c"

MPI_Status status;
MPI_Datatype column_type;
double A[m][n], x[n], b[m], temp_b[m];

int main(int argc, char **argv) {
  int num_procs, my_rank, num_workers, source, dest, cols, cols_per_task, rem_cols, offset;
  int i, j, rc, from_file, print_flag;
  double start, start_node, start_calc;
  double stop, stop_node, stop_calc;
  for (i=0; i<m; i++) {
    b[i] = 0.0;
    temp_b[i] = 0.0;
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  double times[num_procs];
  from_file = atoi(argv[1]);
  print_flag = atoi(argv[2]);
  num_workers = num_procs - 1;
  if (my_rank == 0) {
    if (from_file) {
      read_matrix(m, n, A, argv[3], " ");
      read_vector(n, x, argv[4], " ");
    } else {
      if (print_flag) {
        printf("Perkalian matriks (%dx%d) vektor (%dx1) I . x = b\n", n, n, n);
      }
      init_matrix_i(n, A);
      init_vector(n, x);
    }

    start = MPI_Wtime();
    offset = 0;
    cols_per_task = n / num_workers;
    rem_cols = n % num_workers;
    for (dest=1; dest<=num_workers; dest++) {
      cols = cols_per_task;
      if (dest == num_workers) {
        cols = cols + rem_cols;
      }
      times[dest] = MPI_Wtime();
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
      times[source] = MPI_Wtime() - times[source];
      for (i=0; i<m; i++) {
        b[i] = b[i] + temp_b[i];
      }
    }
    stop = MPI_Wtime();
  } 
  
  if (my_rank > 0) {
    start_node = MPI_Wtime();
    MPI_Recv(&offset, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&cols, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Type_vector(m, cols, n, MPI_DOUBLE, &column_type);
    MPI_Type_commit(&column_type);
    MPI_Recv(&A[0][offset], m*cols, column_type, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&x, n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

    start_calc = MPI_Wtime();
    for (j=offset; j<offset+cols; j++) {
      for (i=0; i<m; i++) {
        b[i] = b[i] + A[i][j] * x[j];
      }
    }
    stop_calc = MPI_Wtime();

    MPI_Send(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    MPI_Send(&cols, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    MPI_Send(&b, m, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
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
      print_vector(m, b);
    } else {
      for(i=1; i<num_procs; i++) {
        printf("Send-Receive time process 0 with process %d is %f s\n", i, times[i]);
      }
      printf("TOTAL TIME : %.9f s\n", stop - start);
    }
  }
  return 0;
}
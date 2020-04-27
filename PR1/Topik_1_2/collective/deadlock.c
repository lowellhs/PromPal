#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include "../helper.c"

MPI_Status status;

int main(int argc, char **argv) {
  int num_procs, my_rank;
  int vec_size = 10000;
  double buf1[vec_size], buf2[vec_size];

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (my_rank == 0) {
    init_vector(vec_size, buf1);
    init_vector(vec_size, buf2);
    MPI_Bcast(&buf1, vec_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&buf2, vec_size, MPI_DOUBLE, 1, MPI_COMM_WORLD);
  } 

  if (my_rank == 1) {
    MPI_Bcast(&buf2, vec_size, MPI_DOUBLE, 1, MPI_COMM_WORLD);
    MPI_Bcast(&buf1, vec_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
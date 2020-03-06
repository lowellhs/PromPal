#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char **argv) {
  int num_procs, my_rank, i;

  double a, b, c;
  srand(time(NULL));
  a = (double)rand()/RAND_MAX;
  b = (double)rand()/RAND_MAX;
  c = (double)rand()/RAND_MAX;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  double my_num = a+b*my_rank+c;
  double global_sum;
  double all_rand_nums[num_procs];
  double all_rand_nums_ex[num_procs];
  MPI_Allreduce(&my_num, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
  MPI_Allgather(&my_num, 1, MPI_DOUBLE, &all_rand_nums, 1, MPI_DOUBLE, MPI_COMM_WORLD);
  MPI_Alltoall(&all_rand_nums, 1, MPI_DOUBLE, &all_rand_nums_ex, 1, MPI_DOUBLE, MPI_COMM_WORLD);
  printf("Process %d, my number : %.6f, all_reduce : %.6f, all_gather : [", my_rank, my_num, global_sum);
  for (i=0; i<num_procs-1; i++) {
    printf("%.6f ", all_rand_nums[i]);
  }
  printf("%.6f], all_to_all [", all_rand_nums[num_procs-1]);
  for (i=0; i<num_procs-1; i++) {
    printf("%.6f ", all_rand_nums_ex[i]);
  }
  printf("%.6f]\n", all_rand_nums_ex[num_procs-1]);

  MPI_Finalize();
  return 0;
}
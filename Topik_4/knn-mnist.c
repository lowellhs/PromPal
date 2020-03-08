#include "mpi.h"
#include "./helper.c"

#define MAX_LEN     100
#define file_train  "test_input/MNIST_train_60k.csv"
#define file_test   "test_input/MNIST_test_10k.csv"
#define rows_train  60000
#define rows_test   10000
#define cols        17
#define K           11
#define num_labels  10

void    sequential(int, int, int);
void    parallel(int, int, int, int, double **);

char    data_train[rows_train][cols][MAX_LEN];
char    data_test[rows_test][cols][MAX_LEN];
double  X_train[rows_train][cols-1];
double  X_test[rows_test][cols-1];
int     y_train[rows_train];
int     y_test[rows_test];
int     y_predict[rows_test];
double  distance_vector[rows_train];
int     indices[rows_train];

int main(int argc, char **argv) {
  double dist, start, comp_time;
  int    i, j, num_procs, my_rank, print_flag, rows, row_start, row_limit;

  print_flag = atoi(argv[1]);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (my_rank == 0) {
    printf("READING TRAIN DATA...\n");
    read_csv(rows_train, cols, MAX_LEN, data_train, file_train);
    for (i=0; i<rows_train; i++) {
      for (j=0; j<cols-1; j++) {
        X_train[i][j] = strtod(data_train[i][j], NULL);
      }
      y_train[i] = atoi(data_train[i][cols-1]);
    }

    printf("READING TEST DATA...\n");
    read_csv(rows_test, cols, MAX_LEN, data_test, file_test);
    for (i=0; i<rows_test; i++) {
      for (j=0; j<cols-1; j++) {
        X_test[i][j] = strtod(data_test[i][j], NULL);
      }
      y_test[i] = atoi(data_test[i][cols-1]);
    }
  }

  if (num_procs == 1) {
    for (i=0; i<rows_test; i++) {
      start = MPI_Wtime();
      sequential(num_procs, my_rank, i);
      comp_time += (MPI_Wtime() - start);
      if (my_rank == 0) {
        if ((i+1) % 250 == 0) {
          printf("Sequential, %d data tested, current total time : %.9f s\n", i+1, comp_time);
        }
      }
    }
  } else {
    start = MPI_Wtime();
    rows = rows_train / num_procs;
    double *recAdata = (double *)malloc(sizeof(double)*rows*(cols-1));
    double **recA = (double **)malloc(sizeof(double *)*rows);
    for (i=0; i<rows; i++) {
      recA[i] = &(recAdata[i*(cols-1)]);
    }
    MPI_Bcast(&X_test, rows_test*(cols-1), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(&X_train, rows*(cols-1), MPI_DOUBLE, &(recA[0][0]), rows*(cols-1), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    comp_time += (MPI_Wtime() - start);
    for (i=0; i<rows_test; i++) {
      start = MPI_Wtime();
      parallel(num_procs, my_rank, i, rows, recA);
      comp_time += (MPI_Wtime() - start);
      if (my_rank == 0) {
        if ((i+1) % 250 == 0) {
          printf("Sequential, %d data tested, current total time : %.9f s\n", i+1, comp_time);
        }
      }
    }
  }

  if (my_rank == 0) {
    if (!print_flag) {
      printf("Accuracy score : %.9f\n", accuracy(rows_test, y_predict, y_test));
    }
  }

  MPI_Finalize();
  return 0;
}

void sequential(int num_procs, int my_rank, int test_idx) {
  int i, selected[K];
  
  init_vector(rows_train, indices);
  for (i=0; i<rows_train; i++) {
    distance_vector[i] = norm_vector(cols-1, X_test[test_idx], X_train[i]);
  }
  mergeSort(distance_vector, indices, 0, rows_train-1);
  for (i=0; i<K; i++) {
    selected[i] = y_train[indices[i]];
  }
  y_predict[test_idx] = major_num(K, num_labels, selected);
}

void parallel(int num_procs, int my_rank, int test_idx, int rows, double **recA) {
  int i, j, selected[K], dist_vec_par_indices[K*num_procs], sendIndices[rows];
  double dist_vec_par[K*num_procs], sendDistVect[rows];
  
  init_vector_part(rows, sendIndices, my_rank*rows);
  for (i=0; i<rows; i++) {
    sendDistVect[i] = norm_vector(cols-1, X_test[test_idx], recA[i]);
  }
  mergeSort(sendDistVect, sendIndices, 0, rows-1);
  MPI_Gather(&sendDistVect, K, MPI_DOUBLE, &dist_vec_par, K, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Gather(&sendIndices, K, MPI_INT, &dist_vec_par_indices, K, MPI_INT, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    mergeSort(dist_vec_par, dist_vec_par_indices, 0, K*num_procs-1);
    for (i=0; i<K; i++) {
      selected[i] = y_train[dist_vec_par_indices[i]];
    }
    y_predict[test_idx] = major_num(K, num_labels, selected);
  }
}

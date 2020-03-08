#include "mpi.h"
#include "./helper.c"
#include "./init.c"

#define MAX_LEN     100
#define K           4
#define num_labels  3
#define cols        5

int     labels_to_num(char[]);
void    sequential(int, int, int);
void    parallel(int, int, int, int, double **, double*);

char    data_train[rows_train][cols][MAX_LEN];
char    data_test[rows_test][cols][MAX_LEN];
double  X_train[rows_train][cols-1];
double  X_test[rows_test][cols-1];
int     y_train[rows_train];
int     y_test[rows_test];
int     y_predict[rows_test];
char    labels[3][MAX_LEN] = { "Iris-setosa", "Iris-versicolor", "Iris-virginica" };
double  distance_vector[rows_train];
int     indices[rows_train];

int main(int argc, char **argv) {
  double dist, start, comp_time, start2, comm_time;
  int    i, j, num_procs, my_rank, print_flag, rows, row_start, row_limit;

  print_flag = atoi(argv[1]);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  
  if (my_rank == 0) {
    // LOAD TRAIN DATA
    read_csv(rows_train, cols, MAX_LEN, data_train, file_train);
    for (i=0; i<rows_train; i++) {
      for (j=0; j<cols-1; j++) {
        X_train[i][j] = strtod(data_train[i][j], NULL);
      }
      y_train[i] = labels_to_num(data_train[i][cols-1]);
    }

    // LOAD TEST DATA
    read_csv(rows_test, cols, MAX_LEN, data_test, file_test);
    for (i=0; i<rows_test; i++) {
      for (j=0; j<cols-1; j++) {
        X_test[i][j] = strtod(data_test[i][j], NULL);
      }
      y_test[i] = labels_to_num(data_test[i][cols-1]);
    }
  }

  if (num_procs == 1) {
    start = MPI_Wtime();
    for (i=0; i<rows_test; i++) {
      sequential(num_procs, my_rank, i);
    }
    comp_time += (MPI_Wtime() - start);
  } else {
    start = MPI_Wtime();
    rows = rows_train / num_procs;
    double *recAdata = (double *)malloc(sizeof(double)*rows*(cols-1));
    double **recA = (double **)malloc(sizeof(double *)*rows);
    for (i=0; i<rows; i++) {
      recA[i] = &(recAdata[i*(cols-1)]);
    }
    start2 = MPI_Wtime();
    MPI_Bcast(&X_test, rows_test*(cols-1), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(&X_train, rows*(cols-1), MPI_DOUBLE, &(recA[0][0]), rows*(cols-1), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    comm_time += (MPI_Wtime() - start2);
    for (i=0; i<rows_test; i++) {
      parallel(num_procs, my_rank, i, rows, recA, &comm_time);
    }
    comp_time += (MPI_Wtime() - start);
  }

  MPI_Finalize();

  if (my_rank == 0) {
    if (!print_flag) {
      print_vector_int(rows_test, y_predict);
    } else {
      char *str_a = "TOTAL TIME : %.9f s, COMM TIME : %.9f s, accuracy : %.4f\n";
      printf(str_a, comp_time, comm_time, accuracy(rows_test, y_predict, y_test));
    }
  }
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

void parallel(int num_procs, int my_rank, int test_idx, int rows, double **recA, double *comm_time) {
  int i, j, selected[K], dist_vec_par_indices[K*num_procs], sendIndices[rows];
  double dist_vec_par[K*num_procs], sendDistVect[rows], start;
  
  init_vector_part(rows, sendIndices, my_rank*rows);
  for (i=0; i<rows; i++) {
    sendDistVect[i] = norm_vector(cols-1, X_test[test_idx], recA[i]);
  }
  mergeSort(sendDistVect, sendIndices, 0, rows-1);
  start = MPI_Wtime();
  MPI_Gather(&sendDistVect, K, MPI_DOUBLE, &dist_vec_par, K, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Gather(&sendIndices, K, MPI_INT, &dist_vec_par_indices, K, MPI_INT, 0, MPI_COMM_WORLD);
  *comm_time = *comm_time + (MPI_Wtime() - start);
  if (my_rank == 0) {
    mergeSort(dist_vec_par, dist_vec_par_indices, 0, K*num_procs-1);
    for (i=0; i<K; i++) {
      selected[i] = y_train[dist_vec_par_indices[i]];
    }
    y_predict[test_idx] = major_num(K, num_labels, selected);
  }
}

int labels_to_num(char *label) {
  int i;
  for (i=0; i<3; i++) {
    if (strcmp(label, labels[i]) == 0) return i;
  }
  return -1;
}

#include "mpi.h"
#include "./helper.c"

#define MAX_LEN     100
#define K           4
#define file_train  "Iris.csv"
#define file_test   "Iris_test_30.csv"
#define num_labels  3
#define rows_train  150
#define rows_test   30
#define cols        5

int     labels_to_num(char[]);
void    sequential(int, int, int);
void    parallel(int, int, int);

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
  double dist, start, comp_time;
  int    i, j, num_procs, my_rank, print_flag;

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
    for (i=0; i<rows_test; i++) {
      start = MPI_Wtime();
      sequential(num_procs, my_rank, i);
      comp_time += (MPI_Wtime() - start);
    }
  } else {
    for (i=0; i<rows_test; i++) {
      start = MPI_Wtime();
      parallel(num_procs, my_rank, i);
      comp_time += (MPI_Wtime() - start);
    }
  }

  MPI_Finalize();

  if (my_rank == 0) {
    if (!print_flag) {
      print_vector_int(rows_test, y_predict);
    } else {
      char *str_a = "TOTAL TIME : %.9f s\n";
      printf(str_a, comp_time);
    }
  }
  return 0;
}

void sequential(int num_procs, int my_rank, int test_idx) {
  int i, selected[K];

  for (i=0; i<rows_train; i++) {
    distance_vector[i] = norm_vector(cols-1, X_test[test_idx], X_train[i]);
  }
  init_vector(rows_train, indices);
  mergeSort(distance_vector, indices, 0, rows_train-1);
  for (i=0; i<K; i++) {
    selected[i] = y_train[indices[i]];
  }
  y_predict[test_idx] = major_num(K, num_labels, selected);
}

void parallel(int num_procs, int my_rank, int test_idx) {
  int i, j, rows, selected[K];

  rows = rows_train / num_procs;
  MPI_Bcast(&X_test[test_idx], cols-1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Scatter(&X_train, rows*(cols-1), MPI_DOUBLE, &X_train[my_rank*rows], rows*(cols-1), MPI_DOUBLE, 0, MPI_COMM_WORLD);

  for (i=my_rank*rows; i<(my_rank*rows+rows); i++) {
    distance_vector[i] = norm_vector(cols-1, X_test[test_idx], X_train[i]);
  }

  MPI_Gather(&distance_vector[my_rank*rows], rows, MPI_DOUBLE, &distance_vector, rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    init_vector(rows_train, indices);
    mergeSort(distance_vector, indices, 0, rows_train-1);
    for (i=0; i<K; i++) {
      selected[i] = y_train[indices[i]];
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

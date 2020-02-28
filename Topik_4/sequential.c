#include "./helper.c"

#define MAX_LEN     100
#define K           4
#define num_labels  3
#define rows_train  150
#define rows_test   3
#define cols        5

int     labels_to_num(char[]);

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
  double dist;
  int    i, j, ii;
  char   *ptr;

  // LOAD TRAIN DATA
  read_csv(rows_train, cols, MAX_LEN, data_train, "Iris.csv");
  for (i=0; i<rows_train; i++) {
    for (j=0; j<cols-1; j++) {
      X_train[i][j] = strtod(data_train[i][j], NULL);
    }
    y_train[i] = labels_to_num(data_train[i][cols-1]);
  }

  // LOAD TEST DATA
  read_csv(rows_test, cols, MAX_LEN, data_test, "Iris_test.csv");
  for (i=0; i<rows_test; i++) {
    for (j=0; j<cols-1; j++) {
      X_test[i][j] = strtod(data_test[i][j], NULL);
    }
    y_test[i] = labels_to_num(data_test[i][cols-1]);
  }

  // PREDICT EVERY TEST DATA
  for (i=0; i<rows_test; i++) {
    int selected[K];
    init_vector(rows_train, indices);
    for (ii=0; ii<rows_train; ii++) {
      distance_vector[ii] = norm_vector(cols-1, X_test[i], X_train[ii]);
    }
    mergeSort(distance_vector, indices, 0, rows_train-1);
    for (ii=0; ii<K; ii++) {
      selected[ii] = y_train[indices[ii]];
    }
    y_predict[i] = major_num(K, num_labels, selected);
  }

  // PRINT PREDICTION
  print_vector_int(rows_test, y_predict);

  return 0;
}

int labels_to_num(char *label) {
  int i;
  for (i=0; i<3; i++) {
    if (strcmp(label, labels[i]) == 0) {
      return i;
    }
  }
  return -1;
}

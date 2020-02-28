#include "./helper.c"

#define MAX_LEN 100
#define rows_train 150
#define rows_test 3
#define cols 5

int     labels_to_num(char[]);

char    data_train[rows_train][cols][MAX_LEN];
char    data_test[rows_test][cols][MAX_LEN];
double  X_train[rows_train][cols-1];
double  X_test[rows_test][cols-1];
int     y_train[rows_train];
int     y_test[rows_test];
char    labels[3][MAX_LEN] = { "Iris-setosa", "Iris-versicolor", "Iris-virginica" };

int main(int argc, char **argv) {
  int i, j;
  char *ptr;

  read_csv(rows_train, cols, MAX_LEN, data_train, "Iris.csv");
  for (i=0; i<rows_train; i++) {
    for (j=0; j<cols-1; j++) {
      X_train[i][j] = strtod(data_train[i][j], NULL);
    }
    y_train[i] = labels_to_num(data_train[i][cols-1]);
  }

  read_csv(rows_test, cols, MAX_LEN, data_test, "Iris_test.csv");
  for (i=0; i<rows_test; i++) {
    for (j=0; j<cols-1; j++) {
      X_test[i][j] = strtod(data_test[i][j], NULL);
    }
    y_test[i] = labels_to_num(data_test[i][cols-1]);
  }

  // for (i=0; i<rows_train; i++) {
  //   for (j=0; j<cols; j++) {
  //     printf("%s ", data_train[i][j]);
  //   }
  //   printf("\n");
  // }
  // for (i=0; i<rows_test; i++) {
  //   for (j=0; j<cols; j++) {
  //     printf("%s ", data_test[i][j]);
  //   }
  //   printf("\n");
  // }

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

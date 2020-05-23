#include "helper.c"

char** mallocData(int m, int n, int maxLen)
{
  char** data = malloc(m * n * sizeof(char*));
  for (int i = 0; i < (m*n); i++) data[i] = malloc(maxLen * sizeof(char));
  return data;
}

void freeData(int m, int n, char **data)
{
  for (int i = 0; i < (m*n); i++) free(data[i]);
  free(data);
}

int main(int *argc, char **argv)
{
  int len = atoi(argv[1]);
  double *arr = (double *)malloc(len*sizeof(double));
  int *idx = (int *)malloc(len*sizeof(int));
  for (int i=0; i<len; i++)
  {
    arr[i] = len-i;
    idx[i] = i;
  }

  mergeSort(arr, idx, 0, len-1);

  /*
  int m = 3, n = 5, maxLen = 80;
  char **data = mallocData(m, n, maxLen);
  
  read_csv(m, n, data, "test_input/Iris_test.csv");
  for (int i=0; i<m; i++)
  {
    for (int j=0; j<n; j++)
    {
      printf("%s\n", data[i*n+j]);
    }
  }
  
  freeData(m, n, data);
  */
  return 0;
}
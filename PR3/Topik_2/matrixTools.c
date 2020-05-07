void randomMatrixGenerator(int n, float *A)
{
  srand(time(0));
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++) 
    {
      A[n*i+j] = rand();
    }
  }
}

void initIdentityMatrix(int n, float *I)
{
  for(int i=0; i<n; i++)
  {
    for(int j=0; j<n; j++)
    {
      if(i==j) I[n*i+j] = 1.0;
      else I[n*i+j] = 0.0;
    }
  } 
}

void printMatrix(int n, float *A)
{
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      printf("%10.3f ", A[n*i+j]);
    }
    printf("\n");
  }
}

float errorMatrix(int n, float *A, float *B)
{
  float res = 0.0;
  for (int i=0; i<n; i++)
  {
    for (int j=0; j<n; j++)
    {
      res += (A[n*i+j] - B[n*i+j]);
    }
  }
  return res;
}

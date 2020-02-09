#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LSIZ 100000
#define RSIZ 100000

void read_file(char fname[20], int n, double A[n][n], double x[n]) {
	char line[RSIZ][LSIZ];
	char *ptr2;
	double value;
	char delim[2] = " ";
	FILE *fptr = NULL; 
	int i = 0;
	int j = 0;
	int tot = 0;

	fptr = fopen(fname, "r");
	while(fgets(line[i], LSIZ, fptr)) 
	{
		line[i][strlen(line[i]) - 1] = '\0';
		i++;
	}
  tot = i;

	ptr2 = strtok(line[0], delim);
	n = atoi(ptr2);

	for (i=1; i <= n; i++) {
		ptr2 = strtok(line[i], delim);
		j = 0;
		while (ptr2 != NULL) {
			value = strtod(ptr2, NULL);
			A[i-1][j] = value;
			ptr2 = strtok(NULL, delim);
			j++;
		}
	}

	ptr2 = strtok(line[n+1], delim);
	j = 0;
	while (ptr2 != NULL) {
		value = strtod(ptr2, NULL);
		x[j] = value;
		ptr2 = strtok(NULL, delim);
		j++;
	}
}

int main(void) 
{
	int n = 3;
	double A[n][n];
	double x[n];

	read_file("file.txt", n, A, x);

	int i, j = 0;

	// printing 2d matrix
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			printf("%f ", A[i][j]);
		}
		printf("%s\n", "");
	}

	// printing vector
	for (i=0; i<n; i++) {
		printf("%f ", x[i]);
	}
	printf("%s\n", "");
}
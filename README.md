# PromPal

## Cara compile :
~~~
./compile.sh
~~~
___
## Contoh tes matriks-vektor :
- Jika ingin melihat hasilnya Ax = b
~~~
mpirun -n 4 ./mpi_matvec_row.o -m 8 -n 4 -m1 a_matrix_8x4.txt -v1 a_vector_4.txt -p 1
~~~
- Jika tidak
~~~
mpirun -n 4 ./mpi_matvec_row.o -m 8 -n 4 -m1 a_matrix_8x4.txt -v1 a_vector_4.txt -p 0
~~~
___
## Contoh tes matriks-matriks :
- Jika ingin melihat hasilnya Ax = b
~~~
mpirun -n 4 ./mpi_matmat_row.o -n 8 -m1 a_matrix_8x8.txt -m1 a_matrix_8x8.txt -p 1
~~~
- Jika tidak
~~~
mpirun -n 4 ./mpi_matmat_row.o -n 8 -m1 a_matrix_8x8.txt -m1 a_matrix_8x8.txt -p 0
~~~
___
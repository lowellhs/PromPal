# PromPal

Contoh run setelah compile :
~~~
mpirun -n 5 ./mpi_matrix_vector.o -n 4 -m data1_matrix_4.txt -v data1_vector_4.txt -p 1
~~~
sintaksnya,
~~~
mpirun -n [#proses] ./mpi_matrix_vector.o -n [ukuran] -m [nama file matrix] -v [nama file vector] -p [1 atau 0]
~~~
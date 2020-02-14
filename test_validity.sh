#!/bin/bash

n=2

d1=collective
d2=point_to_point
d3=sequential

f1=mpi_matvec_row.o
f2=mpi_matvec_col.o
f3=mpi_matmat_row.o
f4=seq_matvec.o
f5=seq_matmat.o

do=test_output

mpirun -n $n $d1/$f1 0 0 > $do/$d1-$f1.txt
mpirun -n $n $d2/$f1 0 0 > $do/$d2-$f1.txt
mpirun -n $n $d2/$f2 0 0 > $do/$d2-$f2.txt
mpirun -n 1 $d3/$f4 0 0 > $do/$d3-$f4.txt

diff $do/$d3-$f4.txt $do/$d1-$f1.txt
diff $do/$d3-$f4.txt $do/$d2-$f1.txt
diff $do/$d3-$f4.txt $do/$d2-$f2.txt

mpirun -n $n $d1/$f3 0 0 > $do/$d1-$f3.txt
mpirun -n $n $d2/$f3 0 0 > $do/$d2-$f3.txt
mpirun -n 1 $d3/$f5 0 0 > $do/$d3-$f5.txt

diff $do/$d3-$f5.txt $do/$d1-$f3.txt
diff $do/$d3-$f5.txt $do/$d2-$f3.txt

rm $do/*.txt
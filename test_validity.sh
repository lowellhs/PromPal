#!/bin/bash

# TESTING WITH DEFAULT FROM PROGRAM
m=$1
n=$2
hostfile=$3

d1=collective
d2=point_to_point
d3=sequential

f1=mpi_matvec_row.o
f2=mpi_matvec_col.o
f3=mpi_matmat_row.o
f4=seq_matvec.o
f5=seq_matmat.o

do=test_output

echo "#define m ${m}"  > init.c
echo "#define n ${n}" >> init.c

bash compile.sh

for i in 2 3 4
do
  echo "Validating..., size ${m}x${n} and ${i} processors"

  mpirun --hostfile $hostfile -np $i $d1/$f1 0 0 > $do/$d1-$f1.txt
  mpirun --hostfile $hostfile -np $i $d1/$f2 0 0 > $do/$d1-$f2.txt
  mpirun --hostfile $hostfile -np $i $d2/$f1 0 0 > $do/$d2-$f1.txt
  mpirun --hostfile $hostfile -np $i $d2/$f2 0 0 > $do/$d2-$f2.txt
  mpirun --hostfile $hostfile -np 1 $d3/$f4 0 0 > $do/$d3-$f4.txt

  diff $do/$d3-$f4.txt $do/$d1-$f1.txt
  diff $do/$d3-$f4.txt $do/$d1-$f2.txt
  diff $do/$d3-$f4.txt $do/$d2-$f1.txt
  diff $do/$d3-$f4.txt $do/$d2-$f2.txt

  mpirun --hostfile $hostfile -np $i $d1/$f3 0 0 > $do/$d1-$f3.txt
  mpirun --hostfile $hostfile -np $i $d2/$f3 0 0 > $do/$d2-$f3.txt
  mpirun --hostfile $hostfile -np 1 $d3/$f5 0 0 > $do/$d3-$f5.txt

  diff $do/$d3-$f5.txt $do/$d1-$f3.txt
  diff $do/$d3-$f5.txt $do/$d2-$f3.txt

  rm $do/*.o.txt
done

rm init.c

# TESTING USING FILE INPUT
for size in 255 256
do
  echo "#define m ${size}"  > init.c
  echo "#define n ${size}" >> init.c
  bash compile.sh
  for i in 2 3 4
  do
    echo "Validating..., size ${size}x${size} and ${i} processors (from input file)"

    mat1="test_input/matrix_i_${size}x${size}.txt"
    mat2="test_input/matrix_${size}x${size}.txt"
    vec1="test_input/vector_${size}x1.txt"

    mpirun --hostfile $hostfile -np $i $d1/$f1 1 0 $mat1 $vec1 > $do/$d1-$f1.txt
    mpirun --hostfile $hostfile -np $i $d1/$f2 1 0 $mat1 $vec1 > $do/$d1-$f2.txt
    mpirun --hostfile $hostfile -np $i $d2/$f1 1 0 $mat1 $vec1 > $do/$d2-$f1.txt
    mpirun --hostfile $hostfile -np $i $d2/$f2 1 0 $mat1 $vec1 > $do/$d2-$f2.txt
    
    diff $vec1 $do/$d1-$f1.txt
    diff $vec1 $do/$d1-$f2.txt
    diff $vec1 $do/$d2-$f1.txt
    diff $vec1 $do/$d2-$f2.txt

    mpirun --hostfile $hostfile -np $i $d1/$f3 1 0 $mat1 $mat2 > $do/$d1-$f3.txt
    mpirun --hostfile $hostfile -np $i $d2/$f3 1 0 $mat1 $mat2 > $do/$d2-$f3.txt
    mpirun --hostfile $hostfile -np $i $d1/$f3 1 0 $mat2 $mat1 > $do/v2-$d1-$f3.txt
    mpirun --hostfile $hostfile -np $i $d2/$f3 1 0 $mat2 $mat1 > $do/v2-$d2-$f3.txt

    diff $mat2 $do/$d1-$f3.txt
    diff $mat2 $do/$d2-$f3.txt
    diff $mat2 $do/v2-$d1-$f3.txt
    diff $mat2 $do/v2-$d2-$f3.txt

    rm $do/*.o.txt
  done
done

rm init.c

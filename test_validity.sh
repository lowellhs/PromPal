#!/bin/bash
m=$1
n=$2
hostfile=$3

echo "#define m ${m}"  > init.c
echo "#define n ${n}" >> init.c

bash compile.sh

for i in 2 3 4
do
  echo "Validating..., size ${m}x${n} and ${i} processors"
  d1=collective
  d2=point_to_point
  d3=sequential

  f1=mpi_matvec_row.o
  f2=mpi_matvec_col.o
  f3=mpi_matmat_row.o
  f4=seq_matvec.o
  f5=seq_matmat.o

  do=test_output

  mpirun --hostfile $hostfile -np $n $d1/$f1 0 0 > $do/$d1-$f1.txt
  mpirun --hostfile $hostfile -np $n $d1/$f2 0 0 > $do/$d1-$f2.txt
  mpirun --hostfile $hostfile -np $n $d2/$f1 0 0 > $do/$d2-$f1.txt
  mpirun --hostfile $hostfile -np $n $d2/$f2 0 0 > $do/$d2-$f2.txt
  mpirun --hostfile $hostfile -np 1 $d3/$f4 0 0 > $do/$d3-$f4.txt

  diff $do/$d3-$f4.txt $do/$d1-$f1.txt
  diff $do/$d3-$f4.txt $do/$d1-$f2.txt
  diff $do/$d3-$f4.txt $do/$d2-$f1.txt
  diff $do/$d3-$f4.txt $do/$d2-$f2.txt

  mpirun --hostfile $hostfile -np $n $d1/$f3 0 0 > $do/$d1-$f3.txt
  mpirun --hostfile $hostfile -np $n $d2/$f3 0 0 > $do/$d2-$f3.txt
  mpirun --hostfile $hostfile -np 1 $d3/$f5 0 0 > $do/$d3-$f5.txt

  diff $do/$d3-$f5.txt $do/$d1-$f3.txt
  diff $do/$d3-$f5.txt $do/$d2-$f3.txt

  rm $do/*.o.txt
  echo "Validated"
done

rm init.c
#!/bin/bash

n=$1
env=$2
hostfile=$3
shift
shift
shift

bash compile.sh $n

A="$PWD/test_input/matrix_A_${n}x${n}.txt"
x="$PWD/test_input/vector_x_${n}x1.txt"
b="$PWD/test_input/vector_b_${n}x1.txt"

seq="$PWD/jacobi/sequential.o"
mpi="$PWD/jacobi/mpi_nonopt.o"

fileOut="$PWD/test_output/${env}_time_${n}.txt"

echo > $fileOut

echo "SEQUENTIAL" >> $fileOut
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $fileOut
echo "$(mpirun --hostfile $hostfile -np 1 $seq 1 1 $A $b)" >> $fileOut
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $fileOut
echo "$(mpirun --hostfile $hostfile -np 1 $seq 1 1 $A $b)" >> $fileOut
echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $fileOut
echo "$(mpirun --hostfile $hostfile -np 1 $seq 1 1 $A $b)" >> $fileOut
echo >> $fileOut

for procs in $@
do
  echo "PROCESSOR $procs"
  echo "PROCESSOR $procs" >> $fileOut
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"      >> $fileOut
  echo "$(mpirun --hostfile $hostfile -np $procs $mpi 1 1 $A $b)" >> $fileOut
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $fileOut
  echo "$(mpirun --hostfile $hostfile -np $procs $mpi 1 1 $A $b)" >> $fileOut
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"      >> $fileOut
  echo "$(mpirun --hostfile $hostfile -np $procs $mpi 1 1 $A $b)" >> $fileOut
  echo >> $fileOut
done

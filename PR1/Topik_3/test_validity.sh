#!/bin/bash

n=$1
env=$2
hostfile=$3
shift
shift
shift

bash compile.sh $n

A="test_input/matrix_A_${n}x${n}.txt"
x="test_input/vector_x_${n}x1.txt"
b="test_input/vector_b_${n}x1.txt"

seq="jacobi/sequential.o"
mpi="jacobi/mpi_nonopt.o"

fileOutSeq="test_output/${env}_1_output_${n}.txt"
echo "$(mpirun --hostfile $hostfile -np 1 $seq 1 0 $A $b)" > $fileOutSeq

for procs in $@
do
  echo "PROCESSOR $procs"
  fileOutPar="test_output/${env}_${procs}_output_${n}.txt"
  echo "$(mpirun --hostfile $hostfile -np $procs $mpi 1 0 $A $b)" > $fileOutPar
done

python3 validation.py $x $env $n


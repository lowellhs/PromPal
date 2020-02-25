#!/bin/bash

m=$1
n=$2
env=$3
hostfile=$4
shift
shift
shift
shift
outDir="test_output"

echo "#define m ${m}"  > init.c
echo "#define n ${n}" >> init.c

bash compile.sh

echo "sequential matrix-vector multiplication"
seqmatvec1=$(mpirun --hostfile $hostfile -np 1 sequential/seq_matvec.o 0 1)
seqmatvec2=$(mpirun --hostfile $hostfile -np 1 sequential/seq_matvec.o 0 1)
seqmatvec3=$(mpirun --hostfile $hostfile -np 1 sequential/seq_matvec.o 0 1)

echo "sequential matrix-matrix multiplication"
seqmatmat1=$(mpirun --hostfile $hostfile -np 1 sequential/seq_matmat.o 0 1)
seqmatmat2=$(mpirun --hostfile $hostfile -np 1 sequential/seq_matmat.o 0 1)
seqmatmat3=$(mpirun --hostfile $hostfile -np 1 sequential/seq_matmat.o 0 1)

for procs in $@
do
  echo "PROCESSORS = ${procs}"
  echo "----------------------------------------------------------------------------------"
  newFile="${outDir}/${env}_test_${m}x${n}_${procs}.txt"
  echo "Test ${m}x${n} matrix with ${procs} processors" > $newFile
  echo "" >> $newFile

  echo "sequential matrix-vector multiplication" >> $newFile
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
  echo "$seqmatvec1" >> $newFile
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
  echo "$seqmatvec2" >> $newFile
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
  echo "$seqmatvec3" >> $newFile
  echo >> $newFile
  
  echo "sequential matrix-matrix multiplication" >> $newFile
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
  echo "$seqmatmat1" >> $newFile
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
  echo "$seqmatmat2" >> $newFile
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
  echo "$seqmatmat3" >> $newFile
  echo >> $newFile

  echo "point-to-point matrix-vector multiplication (row)"
  echo "point-to-point matrix-vector multiplication (row)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun --hostfile $hostfile -np $procs point_to_point/mpi_matvec_row.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "point-to-point matrix-vector multiplication (col)"
  echo "point-to-point matrix-vector multiplication (col)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun --hostfile $hostfile -np $procs point_to_point/mpi_matvec_col.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "point-to-point matrix-matrix multiplication (row)"
  echo "point-to-point matrix-matrix multiplication (row)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun --hostfile $hostfile -np $procs point_to_point/mpi_matmat_row.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "collective matrix-vector multiplication (row)"
  echo "collective matrix-vector multiplication (row)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun --hostfile $hostfile -np $procs collective/mpi_matvec_row.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "collective matrix-vector multiplication (col)"
  echo "collective matrix-vector multiplication (col)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun --hostfile $hostfile -np $procs collective/mpi_matvec_col.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "collective matrix-matrix multiplication (row)"
  echo "collective matrix-matrix multiplication (row)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun --hostfile $hostfile -np $procs collective/mpi_matmat_row.o 0 1)
    echo "$time" >> $newFile
  done
  echo "----------------------------------------------------------------------------------"
done

rm init.c
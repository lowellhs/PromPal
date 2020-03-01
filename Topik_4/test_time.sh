#!/bin/bash

n=$1
env=$2
hostfile=$3
shift
shift
shift

train="test_input/Iris_${n}.csv"
test="test_input/Iris_test.csv"

echo "#define file_train  \"${train}\"" >   init.c
echo "#define file_test   \"${test}\""  >>  init.c
echo "#define rows_train  ${n}"         >>  init.c
echo "#define rows_test   $(cat $test | wc -l)"            >>  init.c

mpicc -o knn.o knn.c -lm

fileOut="test_output/${env}_time_${n}.txt"

rm -f $fileOut
touch $fileOut

for procs in $@
do
  echo "PROCESSOR $procs"
  echo "PROCESSOR $procs" >> $fileOut
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"      >> $fileOut
  echo "$(mpirun --hostfile $hostfile -np $procs knn.o 1)"         >> $fileOut
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"      >> $fileOut
  echo "$(mpirun --hostfile $hostfile -np $procs knn.o 1)"         >> $fileOut
  echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"      >> $fileOut
  echo "$(mpirun --hostfile $hostfile -np $procs knn.o 1)"         >> $fileOut
  echo >> $fileOut
done

#rm init.c
rm knn.o

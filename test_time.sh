m=$1
n=$2
procs=$3

echo "#define m $1"  > init.c
echo "#define n $2" >> init.c

bash compile.sh

for procs in 2 4 8 16
do
  echo "PROCESSORS = ${procs}"
  echo "----------------------------------------------------------------------------------"
  newFile="test_${m}x${n}_${procs}.txt"
  echo "Test ${m}x${n} matrix with ${procs} processors" > $newFile
  echo "" >> $newFile

  echo "sequential matrix-vector multiplication"
  echo "sequential matrix-vector multiplication" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun -n 1 sequential/seq_matvec.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "sequential matrix-matrix multiplication"
  echo "sequential matrix-matrix multiplication" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun -n 1 sequential/seq_matmat.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "point-to-point matrix-vector multiplication (row)"
  echo "point-to-point matrix-vector multiplication (row)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun -n $procs point_to_point/mpi_matvec_row.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "point-to-point matrix-vector multiplication (col)"
  echo "point-to-point matrix-vector multiplication (col)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun -n $procs point_to_point/mpi_matvec_col.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "point-to-point matrix-matrix multiplication (row)"
  echo "point-to-point matrix-matrix multiplication (row)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun -n $procs point_to_point/mpi_matmat_row.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "collective matrix-vector multiplication (row)"
  echo "collective matrix-vector multiplication (row)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun -n $procs collective/mpi_matvec_row.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "collective matrix-vector multiplication (col)"
  echo "collective matrix-vector multiplication (col)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun -n $procs collective/mpi_matvec_col.o 0 1)
    echo "$time" >> $newFile
  done
  echo >> $newFile

  echo "collective matrix-matrix multiplication (row)"
  echo "collective matrix-matrix multiplication (row)" >> $newFile
  for i in 1 2 3
  do
    echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" >> $newFile
    time=$(mpirun -n $procs collective/mpi_matmat_row.o 0 1)
    echo "$time" >> $newFile
  done
  echo "----------------------------------------------------------------------------------"
done
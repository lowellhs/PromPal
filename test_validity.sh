#!/bin/bash

procs=4

o1=test_output/test_validity_matvec_seq.txt
o2=test_output/test_validity_matvec_row.txt
o3=test_output/test_validity_matvec_col.txt
o4=test_output/test_validity_matmat_row.txt
o5=test_output/test_validity_matmat_seq.txt

i1=test_input/a_matrix_8x4.txt
i2=test_input/a_vector_4.txt
i3=test_input/a_matrix_8x8.txt

./seq_matvec.o -m 4 -n 6 -p 1 > ${o1}
./seq_matvec.o -m 6 -n 4 -p 1 >> ${o1}
./seq_matvec.o -m 4 -n 4 -p 1 >> ${o1}
./seq_matvec.o -m 8 -n 4 -m1 ${i1} -v1 ${i2} -p 1 >> ${o1}

mpirun -n ${procs} ./mpi_matvec_row.o -m 4 -n 6 -p 1 > ${o2}
mpirun -n ${procs} ./mpi_matvec_row.o -m 6 -n 4 -p 1 >> ${o2}
mpirun -n ${procs} ./mpi_matvec_row.o -m 4 -n 4 -p 1 >> ${o2}
mpirun -n ${procs} ./mpi_matvec_row.o -m 8 -n 4 -m1 ${i1} -v1 ${i2} -p 1 >> ${o2}

mpirun -n ${procs} ./mpi_matvec_col.o -m 4 -n 6 -p 1 > ${o3}
mpirun -n ${procs} ./mpi_matvec_col.o -m 6 -n 4 -p 1 >> ${o3}
mpirun -n ${procs} ./mpi_matvec_col.o -m 4 -n 4 -p 1 >> ${o3}
mpirun -n ${procs} ./mpi_matvec_col.o -m 8 -n 4 -m1 ${i1} -v1 ${i2} -p 1 >> ${o3}

diff ${o1} ${o2}
diff ${o1} ${o3}

mpirun -n ${procs} ./mpi_matmat_row.o -n 4 -p 1 > ${o4}
mpirun -n ${procs} ./mpi_matmat_row.o -n 8 -m1 ${i3} -m2 ${i3} -p 1 > ${o4}

./seq_matmat.o -n 4 -p 1 > ${o5}
./seq_matmat.o -n 8 -m1 ${i3} -m2 ${i3} -p 1 > ${o5}

diff ${o5} ${o4}
